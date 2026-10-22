#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stddef.h>
#include <stdint.h>

#define HUF_SYMBOLS 256
#define HUF_MAGIC "HUF1"
/* Header: magic(4) + original size(8) + 256 x uint32 frequencies */
#define HUF_HEADER_SIZE (4 + 8 + HUF_SYMBOLS * 4)

typedef enum {
    HUF_OK = 0,
    HUF_ERR_MEMORY,
    HUF_ERR_TOO_LARGE,
    HUF_ERR_BAD_FORMAT,
    HUF_ERR_TRUNCATED
} HufStatus;

typedef struct HuffNode {
    uint32_t freq;
    int symbol;                 /* -1 for internal nodes */
    struct HuffNode *left;
    struct HuffNode *right;
} HuffNode;

typedef struct {
    uint8_t bits[HUF_SYMBOLS];  /* one entry per bit: 0 or 1 */
    int length;
} HuffCode;

void huf_count_freq(const uint8_t *data, size_t n, uint32_t freq[HUF_SYMBOLS]);
HuffNode *huf_build_tree(const uint32_t freq[HUF_SYMBOLS]);
void huf_free_tree(HuffNode *root);
void huf_build_codes(const HuffNode *root, HuffCode codes[HUF_SYMBOLS]);

HufStatus huf_compress(const uint8_t *in, size_t n, uint8_t **out, size_t *out_n);
HufStatus huf_decompress(const uint8_t *in, size_t n, uint8_t **out, size_t *out_n);

const char *huf_strerror(HufStatus s);

#endif
