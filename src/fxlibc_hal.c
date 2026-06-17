#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
int write(int fd, const void *buf, size_t count) { (void)fd; (void)buf; (void)count; return -1; }
off_t lseek(int fd, off_t offset, int whence) { (void)fd; (void)offset; (void)whence; return -1; }
void *fxlibc_hal_malloc(size_t size) { return malloc(size); }
void fxlibc_hal_free(void *ptr) { free(ptr); }
void *fxlibc_hal_realloc(void *ptr, size_t size) { return realloc(ptr, size); }
