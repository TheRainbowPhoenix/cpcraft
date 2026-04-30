//https://github.com/andyherbert/lz1/blob/master/lz.c

unsigned int pow_(unsigned int base, unsigned int exp) {
    if (exp == 0) return 1;
    return 1 << exp;
}

unsigned int lz77_compress (unsigned char *uncompressed_text, unsigned int uncompressed_size, unsigned char *compressed_text, unsigned char pointer_length_width)
{
    unsigned short pointer_pos, temp_pointer_pos, output_pointer, pointer_length, temp_pointer_length;
    unsigned int compressed_pointer, output_size, coding_pos, output_lookahead_ref, look_behind, look_ahead;
    unsigned short pointer_pos_max, pointer_length_max;
    pointer_pos_max = pow_(2, 16 - pointer_length_width);
    pointer_length_max = pow_(2, pointer_length_width);

    *(compressed_text + 0) = ((uncompressed_size&0b11111111000000000000000000000000)>>24);
    *(compressed_text + 1) = ((uncompressed_size&0b00000000111111110000000000000000)>>16);
    *(compressed_text + 2) = ((uncompressed_size&0b00000000000000001111111100000000)>>8);
    *(compressed_text + 3) = ((uncompressed_size&0b00000000000000000000000011111111));

    *(compressed_text + 4) = pointer_length_width;
    compressed_pointer = output_size = 5;
    
    for(coding_pos = 0; coding_pos < uncompressed_size; ++coding_pos)
    {
        pointer_pos = 0;
        pointer_length = 0;
        for(temp_pointer_pos = 1; (temp_pointer_pos < pointer_pos_max) && (temp_pointer_pos <= coding_pos); ++temp_pointer_pos)
        {
            look_behind = coding_pos - temp_pointer_pos;
            look_ahead = coding_pos;
            for(temp_pointer_length = 0; uncompressed_text[look_ahead++] == uncompressed_text[look_behind++]; ++temp_pointer_length)
                if(temp_pointer_length == pointer_length_max)
                    break;
            if(temp_pointer_length > pointer_length)
            {
                pointer_pos = temp_pointer_pos;
                pointer_length = temp_pointer_length;
                if(pointer_length == pointer_length_max)
                    break;
            }
        }
        coding_pos += pointer_length;
        if((coding_pos == uncompressed_size) && pointer_length)
        {
            output_pointer = (pointer_length == 1) ? 0 : ((pointer_pos << pointer_length_width) | (pointer_length - 2));
            output_lookahead_ref = coding_pos - 1;
        }
        else
        {
            output_pointer = (pointer_pos << pointer_length_width) | (pointer_length ? (pointer_length - 1) : 0);
            output_lookahead_ref = coding_pos;
        }
        *(compressed_text + compressed_pointer + 0) = ((output_pointer&0b1111111100000000)>>8);
        *(compressed_text + compressed_pointer + 1) = ((output_pointer&0b0000000011111111));

        compressed_pointer += 2;
        *(compressed_text + compressed_pointer++) = *(uncompressed_text + output_lookahead_ref);
        output_size += 3;
    }

    return output_size;
}

unsigned int lz77_decompress (unsigned char *compressed_text, unsigned char *uncompressed_text)
{
    unsigned char pointer_length_width;
    unsigned short input_pointer, pointer_length, pointer_pos, pointer_length_mask;
    unsigned int compressed_pointer, coding_pos, pointer_offset, uncompressed_size;

    //uncompressed_size = *((unsigned int *) compressed_text);
    uncompressed_size = ((*(compressed_text + 0) << 24) | (*(compressed_text + 1) << 16) | (*(compressed_text + 2) << 8) | *(compressed_text + 3));
    pointer_length_width = *(compressed_text + 4);
    compressed_pointer = 5;

    pointer_length_mask = pow_(2, pointer_length_width) - 1;

    for(coding_pos = 0; coding_pos < uncompressed_size; ++coding_pos)
    {
        //input_pointer = *((unsigned short *) (compressed_text + compressed_pointer));
        input_pointer = ((*(compressed_text + compressed_pointer + 0) << 8) | (*(compressed_text + compressed_pointer + 1)));
        compressed_pointer += 2;
        pointer_pos = input_pointer >> pointer_length_width;
        pointer_length = pointer_pos ? ((input_pointer & pointer_length_mask) + 1) : 0;
        if(pointer_pos)
            for(pointer_offset = coding_pos - pointer_pos; pointer_length > 0; --pointer_length)
                uncompressed_text[coding_pos++] = uncompressed_text[pointer_offset++];
        *(uncompressed_text + coding_pos) = *(compressed_text + compressed_pointer++);
    }

    return coding_pos;
}