#include "huffman.h"

#include <stdlib.h>
#include <string.h>

void huf_count_freq(const uint8_t *data, size_t n, uint32_t freq[HUF_SYMBOLS])
{
    memset(freq, 0, HUF_SYMBOLS * sizeof(uint32_t));
    for (size_t i = 0; i < n; i++)
        freq[data[i]]++;
}

static HuffNode *new_node(uint32_t freq, int symbol, HuffNode *l, HuffNode *r)
{
    HuffNode *n = malloc(sizeof(HuffNode));
    if (!n) return NULL;
    n->freq = freq;
    n->symbol = symbol;
    n->left = l;
    n->right = r;
    return n;
}

void huf_free_tree(HuffNode *root)
{
    if (!root) return;
    huf_free_tree(root->left);
    huf_free_tree(root->right);
    free(root);
}

/* Deterministic: the decoder rebuilds the identical tree from the frequencies. */
HuffNode *huf_build_tree(const uint32_t freq[HUF_SYMBOLS])
{
    HuffNode *pool[HUF_SYMBOLS];
    int count = 0;

    for (int s = 0; s < HUF_SYMBOLS; s++) {
        if (freq[s] > 0) {
            pool[count] = new_node(freq[s], s, NULL, NULL);
            if (!pool[count]) goto fail;
            count++;
        }
    }
    if (count == 0) return NULL;

    while (count > 1) {
        int a = 0, b = 1;               /* indices of two smallest */
        if (pool[b]->freq < pool[a]->freq) { a = 1; b = 0; }
        for (int i = 2; i < count; i++) {
            if (pool[i]->freq < pool[a]->freq)      { b = a; a = i; }
            else if (pool[i]->freq < pool[b]->freq) { b = i; }
        }
        HuffNode *parent = new_node(pool[a]->freq + pool[b]->freq, -1,
                                    pool[a], pool[b]);
        if (!parent) goto fail;
        int lo = a < b ? a : b, hi = a < b ? b : a;
        pool[lo] = parent;
        pool[hi] = pool[count - 1];
        count--;
    }
    return pool[0];

fail:
    for (int i = 0; i < count; i++) huf_free_tree(pool[i]);
    return NULL;
}

static void walk(const HuffNode *n, uint8_t *path, int depth, HuffCode codes[])
{
    if (!n->left && !n->right) {
        HuffCode *c = &codes[n->symbol];
        if (depth == 0) {               /* single-symbol input: use 1 bit */
            c->bits[0] = 0;
            c->length = 1;
        } else {
            memcpy(c->bits, path, (size_t)depth);
            c->length = depth;
        }
        return;
    }
    path[depth] = 0; walk(n->left, path, depth + 1, codes);
    path[depth] = 1; walk(n->right, path, depth + 1, codes);
}

void huf_build_codes(const HuffNode *root, HuffCode codes[HUF_SYMBOLS])
{
    uint8_t path[HUF_SYMBOLS];
    memset(codes, 0, HUF_SYMBOLS * sizeof(HuffCode));
    if (root) walk(root, path, 0, codes);
}

static void put_u32(uint8_t *p, uint32_t v) { for (int i = 0; i < 4; i++) p[i] = (uint8_t)(v >> (8 * i)); }
static void put_u64(uint8_t *p, uint64_t v) { for (int i = 0; i < 8; i++) p[i] = (uint8_t)(v >> (8 * i)); }
static uint32_t get_u32(const uint8_t *p) { uint32_t v = 0; for (int i = 0; i < 4; i++) v |= (uint32_t)p[i] << (8 * i); return v; }
static uint64_t get_u64(const uint8_t *p) { uint64_t v = 0; for (int i = 0; i < 8; i++) v |= (uint64_t)p[i] << (8 * i); return v; }

HufStatus huf_compress(const uint8_t *in, size_t n, uint8_t **out, size_t *out_n)
{
    uint32_t freq[HUF_SYMBOLS];
    HuffCode *codes = NULL;
    HuffNode *root = NULL;
    HufStatus st = HUF_OK;

    if (n > UINT32_MAX) return HUF_ERR_TOO_LARGE;

    huf_count_freq(in, n, freq);
    root = huf_build_tree(freq);
    if (n > 0 && !root) return HUF_ERR_MEMORY;

    codes = malloc(HUF_SYMBOLS * sizeof(HuffCode));
    if (!codes) { st = HUF_ERR_MEMORY; goto done; }
    huf_build_codes(root, codes);

    uint64_t total_bits = 0;
    for (int s = 0; s < HUF_SYMBOLS; s++)
        total_bits += (uint64_t)freq[s] * (uint64_t)codes[s].length;

    size_t payload = (size_t)((total_bits + 7) / 8);
    uint8_t *buf = calloc(HUF_HEADER_SIZE + payload + 1, 1);
    if (!buf) { st = HUF_ERR_MEMORY; goto done; }

    memcpy(buf, HUF_MAGIC, 4);
    put_u64(buf + 4, (uint64_t)n);
    for (int s = 0; s < HUF_SYMBOLS; s++)
        put_u32(buf + 12 + 4 * s, freq[s]);

    uint8_t *p = buf + HUF_HEADER_SIZE;
    size_t bitpos = 0;
    for (size_t i = 0; i < n; i++) {
        const HuffCode *c = &codes[in[i]];
        for (int b = 0; b < c->length; b++, bitpos++)
            if (c->bits[b]) p[bitpos >> 3] |= (uint8_t)(0x80 >> (bitpos & 7));
    }

    *out = buf;
    *out_n = HUF_HEADER_SIZE + payload;
done:
    free(codes);
    huf_free_tree(root);
    return st;
}

HufStatus huf_decompress(const uint8_t *in, size_t n, uint8_t **out, size_t *out_n)
{
    if (n < HUF_HEADER_SIZE || memcmp(in, HUF_MAGIC, 4) != 0)
        return HUF_ERR_BAD_FORMAT;

    uint64_t orig = get_u64(in + 4);
    uint32_t freq[HUF_SYMBOLS];
    uint64_t sum = 0;
    for (int s = 0; s < HUF_SYMBOLS; s++) {
        freq[s] = get_u32(in + 12 + 4 * s);
        sum += freq[s];
    }
    if (sum != orig) return HUF_ERR_BAD_FORMAT;

    uint8_t *buf = malloc((size_t)orig + 1);
    if (!buf) return HUF_ERR_MEMORY;

    if (orig == 0) { *out = buf; *out_n = 0; return HUF_OK; }

    HuffNode *root = huf_build_tree(freq);
    if (!root) { free(buf); return HUF_ERR_MEMORY; }

    const uint8_t *p = in + HUF_HEADER_SIZE;
    size_t avail_bits = (n - HUF_HEADER_SIZE) * 8;
    size_t bitpos = 0;

    for (size_t i = 0; i < (size_t)orig; i++) {
        const HuffNode *cur = root;
        if (!cur->left && !cur->right) {          /* single-symbol input */
            if (bitpos >= avail_bits) goto trunc;
            bitpos++;
        } else {
            while (cur->left || cur->right) {
                if (bitpos >= avail_bits) goto trunc;
                int bit = (p[bitpos >> 3] >> (7 - (bitpos & 7))) & 1;
                bitpos++;
                cur = bit ? cur->right : cur->left;
            }
        }
        buf[i] = (uint8_t)cur->symbol;
    }

    huf_free_tree(root);
    *out = buf;
    *out_n = (size_t)orig;
    return HUF_OK;

trunc:
    huf_free_tree(root);
    free(buf);
    return HUF_ERR_TRUNCATED;
}

const char *huf_strerror(HufStatus s)
{
    switch (s) {
    case HUF_OK:             return "ok";
    case HUF_ERR_MEMORY:     return "out of memory";
    case HUF_ERR_TOO_LARGE:  return "input too large (max 4 GiB)";
    case HUF_ERR_BAD_FORMAT: return "not a valid compressed file";
    case HUF_ERR_TRUNCATED:  return "compressed data is truncated";
    }
    return "unknown error";
}
