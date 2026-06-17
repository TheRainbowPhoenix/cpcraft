#include "compression.h"
unsigned int lz77_compress(unsigned char *uncompressed_text, unsigned int uncompressed_size, unsigned char *compressed_text, unsigned char pointer_length_width) {
    (void)pointer_length_width;
    *(compressed_text + 0) = (unsigned char)((uncompressed_size & 0xFF000000) >> 24);
    *(compressed_text + 1) = (unsigned char)((uncompressed_size & 0x00FF0000) >> 16);
    *(compressed_text + 2) = (unsigned char)((uncompressed_size & 0x0000FF00) >> 8);
    *(compressed_text + 3) = (unsigned char)(uncompressed_size & 0x000000FF);
    unsigned int output_pointer = 4, input_pointer = 0;
    while (input_pointer < uncompressed_size) {
        unsigned int match_length = 0, match_distance = 0;
        for (unsigned int distance = 1; distance <= 4095 && distance <= input_pointer; distance++) {
            unsigned int length = 0;
            while (length < 15 && input_pointer + length < uncompressed_size && uncompressed_text[input_pointer + length] == uncompressed_text[input_pointer - distance + length]) length++;
            if (length > match_length) { match_length = length; match_distance = distance; }
        }
        if (match_length >= 3) {
            *(compressed_text + output_pointer) = (unsigned char)((match_distance >> 4) & 0xFF);
            *(compressed_text + output_pointer + 1) = (unsigned char)(((match_distance & 0xF) << 4) | (match_length & 0xF));
            output_pointer += 2; input_pointer += match_length;
        } else {
            *(compressed_text + output_pointer) = uncompressed_text[input_pointer];
            output_pointer++; input_pointer++;
        }
    }
    return output_pointer;
}
unsigned int lz77_decompress(unsigned char *compressed_text, unsigned char *uncompressed_text) {
    unsigned int uncompressed_size = (compressed_text[0] << 24) | (compressed_text[1] << 16) | (compressed_text[2] << 8) | compressed_text[3];
    unsigned int input_pointer = 4, output_pointer = 0;
    while (output_pointer < uncompressed_size) {
        unsigned char byte1 = compressed_text[input_pointer], byte2 = compressed_text[input_pointer + 1];
        unsigned int distance = (byte1 << 4) | (byte2 >> 4), length = byte2 & 0xF;
        if (distance > 0 && length >= 3) {
            for (unsigned int i = 0; i < length; i++) { uncompressed_text[output_pointer] = uncompressed_text[output_pointer - distance]; output_pointer++; }
            input_pointer += 2;
        } else { uncompressed_text[output_pointer] = compressed_text[input_pointer]; output_pointer++; input_pointer++; }
    }
    return output_pointer;
}
