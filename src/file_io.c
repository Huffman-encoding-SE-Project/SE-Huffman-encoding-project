#include "file_io.h"

#include <stdio.h>
#include <stdlib.h>

int read_file(const char *path, uint8_t **data, size_t *size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return -1;

    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return -1; }
    long len = ftell(f);
    if (len < 0 || fseek(f, 0, SEEK_SET) != 0) { fclose(f); return -1; }

    uint8_t *buf = malloc((size_t)len + 1);
    if (!buf) { fclose(f); return -1; }

    size_t got = fread(buf, 1, (size_t)len, f);
    fclose(f);
    if (got != (size_t)len) { free(buf); return -1; }

    *data = buf;
    *size = (size_t)len;
    return 0;
}

int write_file(const char *path, const uint8_t *data, size_t size)
{
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    size_t put = size ? fwrite(data, 1, size, f) : 0;
    int rc = fclose(f);
    return (put == size && rc == 0) ? 0 : -1;
}
