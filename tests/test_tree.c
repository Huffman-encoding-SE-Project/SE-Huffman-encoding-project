
#include <stdio.h>
#include <stdint.h>
#include "../src/huffman.h"

int main(void)
{
    uint32_t freq[HUF_SYMBOLS] = {0};

    freq['A'] = 3;
    freq['B'] = 1;
    freq['C'] = 1;

    HuffNode *root = huf_build_tree(freq);

    if (root == NULL) {
        printf("Tree construction failed\n");
        return 1;
    }

    printf("Root frequency: %u\n", root->freq);
    printf("Tree construction successful\n");

    huf_free_tree(root);
    return 0;
}
