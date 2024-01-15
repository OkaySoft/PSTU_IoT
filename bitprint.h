#ifndef _BIT_PRINT_
#define _BIT_PRINT_

static void bitprint(char *p_dst, unsigned char byte) {
    int i;
    for(i = 0; i < 8; i++)
        p_dst[i] = ((1 << i) & byte) ? '1' : '0';

    p_dst[i] = '\0';
}

#endif
