#ifndef COMPRESSION_H
#define COMPRESSION_H
unsigned int lz77_compress(unsigned char *uncompressed_text, unsigned int uncompressed_size, unsigned char *compressed_text, unsigned char pointer_length_width);
unsigned int lz77_decompress(unsigned char *compressed_text, unsigned char *uncompressed_text);
#endif
