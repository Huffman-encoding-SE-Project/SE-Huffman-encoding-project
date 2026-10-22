#ifndef FILE_IO_H
#define FILE_IO_H

#include <stddef.h>
#include <stdint.h>

/* Return 0 on success, -1 on failure (errno-style message printed by caller). */
int read_file(const char *path, uint8_t **data, size_t *size);
int write_file(const char *path, const uint8_t *data, size_t size);

#endif
