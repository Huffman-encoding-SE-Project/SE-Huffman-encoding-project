
#include <stdio.h>
#include <stdint.h>
#include "../src/huffman.h"

int main(void)
{
    uint32_t freq[HUF_SYMBOLS] = {0};
    HuffCode codes[HUF_SYMBOLS];

    freq['A'] = 3;
    freq['B'] = 1;
    freq['C'] = 1;

    HuffNode *root = huf_build_tree(freq);
    if (root == NULL) {
        printf("Tree construction failed\n");
        return 1;
    }

    huf_build_codes(root, codes);

    printf("A: length = %u\n", codes['A'].length);
    printf("B: length = %u\n", codes['B'].length);
    printf("C: length = %u\n", codes['C'].length);

    huf_free_tree(root);
    return 0;
}
