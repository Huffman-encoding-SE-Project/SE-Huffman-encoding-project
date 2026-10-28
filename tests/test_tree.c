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
    if (!load_input(argc, argv, &data, &size)) return 2;
    huf_count_freq(data, size, freq);
    HuffNode *root = huf_build_tree(freq);
    free(data);
    if (size == 0) {
        if (root != NULL) { huf_free_tree(root); fprintf(stderr, "Empty input should have no tree\n"); return 1; }
        puts("Tree test passed: empty input has no tree");
        return 0;
    }
    if (!root || root->freq != size) {
        huf_free_tree(root);
        fprintf(stderr, "Tree root frequency mismatch\n");
        return 1;
    }
    printf("Tree test passed: root frequency = %u\n", root->freq);
    huf_free_tree(root);
    return 0;
}
