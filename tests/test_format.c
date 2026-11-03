
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../src/huffman.h"
#include "../src/file_io.h"

static uint32_t get_u32(const uint8_t *p)
{
    uint32_t value = 0;
    for (int i = 0; i < 4; i++)
        value |= (uint32_t)p[i] << (8 * i);
    return value;
}

static uint64_t get_u64(const uint8_t *p)
{
    uint64_t value = 0;
    for (int i = 0; i < 8; i++)
        value |= (uint64_t)p[i] << (8 * i);
    return value;
}

int main(int argc, char **argv)
{
    uint8_t *input = NULL;
    size_t input_size = 0;
    uint8_t *compressed = NULL;
    size_t compressed_size = 0;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 2;
    }

    if (read_file(argv[1], &input, &input_size) != 0) {
        fprintf(stderr, "Cannot read input file\n");
        return 1;
    }

    HufStatus status = huf_compress(
        input, input_size, &compressed, &compressed_size
    );

    if (status != HUF_OK) {
        fprintf(stderr, "Compression failed: %s\n", huf_strerror(status));
        free(input);
        return 1;
    }

    int failed = 0;

    if (compressed_size < HUF_HEADER_SIZE) {
        fprintf(stderr, "Compressed header is incomplete\n");
        failed = 1;
        goto cleanup;
    }

    if (memcmp(compressed, HUF_MAGIC, 4) != 0) {
        fprintf(stderr, "Incorrect magic signature\n");
        failed = 1;
        goto cleanup;
    }

    if (get_u64(compressed + 4) != input_size) {
        fprintf(stderr, "Original size mismatch in header\n");
        failed = 1;
        goto cleanup;
    }

    uint32_t expected_freq[HUF_SYMBOLS];
    huf_count_freq(input, input_size, expected_freq);

    for (int i = 0; i < HUF_SYMBOLS; i++) {
        uint32_t stored = get_u32(compressed + 12 + 4 * i);

        if (stored != expected_freq[i]) {
            fprintf(stderr, "Frequency mismatch for byte 0x%02X\n", i);
            failed = 1;
            break;
        }
    }
    if (!failed) {
        HuffNode *root = huf_build_tree(expected_freq);
        HuffCode codes[HUF_SYMBOLS] = {0};
        huf_build_codes(root, codes);

        uint64_t total_bits = 0;

        for (int i = 0; i < HUF_SYMBOLS; i++) {
            total_bits += (uint64_t)expected_freq[i] * codes[i].length;
        }

        size_t expected_payload_size =
            (size_t)((total_bits + 7) / 8);

        if (compressed_size != HUF_HEADER_SIZE + expected_payload_size) {
            fprintf(stderr, "Compressed payload size mismatch\n");
            failed = 1;
        } else {
            uint8_t *expected_payload =
                calloc(expected_payload_size ? expected_payload_size : 1, 1);

            if (expected_payload == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                failed = 1;
            } else {
                size_t bitpos = 0;

                for (size_t i = 0; i < input_size; i++) {
                    HuffCode *code = &codes[input[i]];

                    for (int b = 0; b < code->length; b++) {
                        if (code->bits[b]) {
                            expected_payload[bitpos >> 3] |=
                                (uint8_t)(0x80 >> (bitpos & 7));
                        }
                        bitpos++;
                    }
                }

                if (memcmp(compressed + HUF_HEADER_SIZE,
                        expected_payload,
                        expected_payload_size) != 0) {
                    fprintf(stderr, "Bit-level payload mismatch\n");
                    failed = 1;
                }

                free(expected_payload);
            }
        }

        huf_free_tree(root);
    }

    if (!failed) {
        printf("Format test passed: header, frequency table, and bitstream verified\n");
    }

cleanup:
    free(input);
    free(compressed);
    return failed ? 1 : 0;
}
