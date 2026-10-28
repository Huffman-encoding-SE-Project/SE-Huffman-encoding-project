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
    uint64_t sum = 0;
    for (int i = 0; i < HUF_SYMBOLS; i++) {
        if (freq[i]) printf("byte 0x%02X (%c): %u\n", i,
                            (i >= 32 && i <= 126) ? i : '.', freq[i]);
        sum += freq[i];
    }
    free(data);
    if (sum != size) { fprintf(stderr, "Frequency total mismatch\n"); return 1; }
    printf("Frequency count passed: %zu bytes\n", size);
    return 0;
}
