
#include <stdio.h>
#include <stdint.h>
#include "../src/huffman.h"

int main(void)
{
    uint8_t data[] = {'A', 'B', 'A', 'C', 'A'};
    uint32_t freq[HUF_SYMBOLS];

    huf_count_freq(data, sizeof(data), freq);

    printf("A: %u\n", freq['A']);
    printf("B: %u\n", freq['B']);
    printf("C: %u\n", freq['C']);

    return 0;
}
