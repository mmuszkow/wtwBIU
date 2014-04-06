#include "stdinc.h"

#include "ggHeaders.h"

char *gg_cp_to_utf8(const char *b)
{
    unsigned char *buf = (unsigned char *) b;
    char *newbuf;
    int newlen = 0;
    int i, j;

    for (i = 0; buf[i]; i++) {
            WORD znak = (buf[i] < 0x80) ? buf[i] : table_cp1250[buf[i]-0x80];

            if (znak < 0x80)        newlen += 1;
            else if (znak < 0x800)  newlen += 2;
            else                    newlen += 3;
    }

    if (!(newbuf = (char*)malloc(newlen+1))) {
            return NULL;
    }

    for (i = 0, j = 0; buf[i]; i++) {
            WORD znak = (buf[i] < 0x80) ? buf[i] : table_cp1250[buf[i]-0x80];
            int count;

            if (znak < 0x80)        count = 1;
            else if (znak < 0x800)  count = 2;
            else                    count = 3;

            switch (count) {
                    case 3: newbuf[j+2] = 0x80 | (znak & 0x3f); znak = znak >> 6; znak |= 0x800;
                    case 2: newbuf[j+1] = 0x80 | (znak & 0x3f); znak = znak >> 6; znak |= 0xc0;
                    case 1: newbuf[j] = (char)znak;
            }
            j += count;
    }
    newbuf[j] = '\0';

    return newbuf;
}