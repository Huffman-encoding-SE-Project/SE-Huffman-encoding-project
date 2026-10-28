#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../src/huffman.h"
#include "../src/file_io.h"

static int load_input(int argc, char **argv, uint8_t **data, size_t *size)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 0;
    }
    if (read_file(argv[1], data, size) != 0) {
        fprintf(stderr, "Cannot read input file: %s\n", argv[1]);
        return 0;
    }
    return 1;
}
\
int main(int argc, char **argv)
{
    uint8_t *data = NULL;
    size_t size = 0;
    uint32_t freq[HUF_SYMBOLS];
    HuffCode codes[HUF_SYMBOLS];
    if (!load_input(argc, argv, &data, &size)) return 2;
    huf_count_freq(data, size, freq);
    HuffNode *root = huf_build_tree(freq);
    huf_build_codes(root, codes);
    int failed = 0;
    for (int i = 0; i < HUF_SYMBOLS; i++) {
        if (freq[i] && codes[i].length <= 0) {
            fprintf(stderr, "Missing code for byte 0x%02X\n", i);
            failed = 1;
        }
    }
    if (size == 0 && root != NULL) failed = 1;
    if (size > 0 && root == NULL) failed = 1;
    if (!failed) printf("Code test passed: valid codes for all symbols in %zu bytes\n", size);
    free(data);
    huf_free_tree(root);
    return failed ? 1 : 0;
}
