#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_io.h"
#include "huffman.h"

static void usage(const char *prog)
{
    fprintf(stderr,
            "Huffman Compression Tool\n"
            "Usage:\n"
            "  %s compress   <input> <output>\n"
            "  %s decompress <input> <output>\n",
            prog, prog);
}

int main(int argc, char **argv)
{
    if (argc != 4) { usage(argv[0]); return 2; }

    int compress;
    if (strcmp(argv[1], "compress") == 0)        compress = 1;
    else if (strcmp(argv[1], "decompress") == 0) compress = 0;
    else { usage(argv[0]); return 2; }

    uint8_t *in = NULL, *out = NULL;
    size_t in_n = 0, out_n = 0;

    if (read_file(argv[2], &in, &in_n) != 0) {
        fprintf(stderr, "error: cannot read '%s'\n", argv[2]);
        return 1;
    }

    HufStatus st = compress ? huf_compress(in, in_n, &out, &out_n)
                            : huf_decompress(in, in_n, &out, &out_n);
    free(in);
    if (st != HUF_OK) {
        fprintf(stderr, "error: %s\n", huf_strerror(st));
        return 1;
    }

    if (write_file(argv[3], out, out_n) != 0) {
        fprintf(stderr, "error: cannot write '%s'\n", argv[3]);
        free(out);
        return 1;
    }
    free(out);

    if (compress && in_n > 0)
        printf("%zu -> %zu bytes (%.1f%% of original)\n",
               in_n, out_n, 100.0 * (double)out_n / (double)in_n);
    else
        printf("%zu -> %zu bytes\n", in_n, out_n);
    return 0;
}
