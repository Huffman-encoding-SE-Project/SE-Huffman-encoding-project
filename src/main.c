#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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


static int has_extension(const char *path, const char *extension)
{
    const char *dot = strrchr(path, '.');
    const char *slash = strrchr(path, '/');
    const char *backslash = strrchr(path, '\\');

    if (slash == NULL || (backslash != NULL && backslash > slash))
        slash = backslash;

    if (dot == NULL || (slash != NULL && dot < slash))
        return 0;

    while (*dot && *extension) {
        if (tolower((unsigned char)*dot) !=
            tolower((unsigned char)*extension))
            return 0;

        dot++;
        extension++;
    }

    return *dot == '\0' && *extension == '\0';
}

static int same_path(const char *path1, const char *path2)
{
    while (*path1 && *path2) {
        if (tolower((unsigned char)*path1) !=
            tolower((unsigned char)*path2))
            return 0;

        path1++;
        path2++;
    }

    return *path1 == '\0' && *path2 == '\0';
}

static int confirm_overwrite(const char *path)
{
    FILE *file = fopen(path, "rb");

    if (file == NULL)
        return 1;

    fclose(file);

    char choice;

    while (1) {
        printf("Output file already exists: %s\n", path);
        printf("Do you want to overwrite it? (y/n): ");

        if (scanf(" %c", &choice) != 1)
            return 0;

        if (choice == 'y' || choice == 'Y')
            return 1;

        if (choice == 'n' || choice == 'N')
            return 0;

        printf("Invalid choice. Enter y or n.\n");
    }
}

int main(int argc, char **argv)
{
    if (argc != 4) { usage(argv[0]); return 2; }

    int compress;
    if (strcmp(argv[1], "compress") == 0)        compress = 1;
    else if (strcmp(argv[1], "decompress") == 0) compress = 0;
    else { usage(argv[0]); return 2; }

    if (same_path(argv[2], argv[3])) {
        fprintf(stderr, "error: input and output paths cannot be the same\n");
        return 1;
    }
    
    if (!confirm_overwrite(argv[3])) {
        printf("Operation cancelled.\n");
        return 0;
    }
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
