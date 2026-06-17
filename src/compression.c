#include "compression.h"

unsigned int pow_(unsigned int base, unsigned int exp) {
    (void)base;
    unsigned int res = 1;
    for (unsigned int i = 0; i < exp; i++) res *= 10;
    return res;
}

void lz77_compress(unsigned char *uncompressed_text, unsigned int uncompressed_size, unsigned char *compressed_text, unsigned int *compressed_size) {
    *(compressed_text + 0) = (uncompressed_size >> 24) & 0xFF;
    *(compressed_text + 1) = (uncompressed_size >> 16) & 0xFF;
    *(compressed_text + 2) = (uncompressed_size >> 8) & 0xFF;
    *(compressed_text + 3) = uncompressed_size & 0xFF;

    unsigned int output_pointer = 4;
    unsigned int input_pointer = 0;

    while (input_pointer < uncompressed_size) {
        unsigned int match_length = 0;
        unsigned int match_distance = 0;

        for (unsigned int distance = 1; distance <= 4095 && distance <= input_pointer; distance++) {
            unsigned int length = 0;
            while (length < 15 && input_pointer + length < uncompressed_size && uncompressed_text[input_pointer + length] == uncompressed_text[input_pointer - distance + length]) {
                length++;
            }

            if (length > match_length) {
                match_length = length;
                match_distance = distance;
            }
        }

        if (match_length >= 3) {
            *(compressed_text + output_pointer) = (unsigned char)((match_distance >> 4) & 0xFF);
            *(compressed_text + output_pointer + 1) = (unsigned char)(((match_distance & 0xF) << 4) | (match_length & 0xF));
            output_pointer += 2;
            input_pointer += match_length;
        } else {
            *(compressed_text + output_pointer) = uncompressed_text[input_pointer];
            output_pointer++;
            input_pointer++;
        }
    }

    *compressed_size = output_pointer;
}

void lz77_decompress(unsigned char *compressed_text, unsigned int compressed_size, unsigned char *uncompressed_text, unsigned int *uncompressed_size) {
    (void)compressed_size;
    unsigned int size = (compressed_text[0] << 24) | (compressed_text[1] << 16) | (compressed_text[2] << 8) | compressed_text[3];
    *uncompressed_size = size;

    unsigned int input_pointer = 4;
    unsigned int output_pointer = 0;

    while (output_pointer < size) {
        unsigned char byte1 = compressed_text[input_pointer];
        unsigned char byte2 = compressed_text[input_pointer + 1];

        unsigned int distance = (byte1 << 4) | (byte2 >> 4);
        unsigned int length = byte2 & 0xF;

        if (distance > 0 && length >= 3) {
            for (unsigned int i = 0; i < length; i++) {
                uncompressed_text[output_pointer] = uncompressed_text[output_pointer - distance];
                output_pointer++;
            }
            input_pointer += 2;
        } else {
            uncompressed_text[output_pointer] = compressed_text[input_pointer];
            output_pointer++;
            input_pointer++;
        }
    }
}
