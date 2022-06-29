#include "bean.h"

unsigned char Crc8(unsigned char *pcBlock, unsigned char len)
{
    unsigned char crc = 0x00;
    unsigned char i;

    while (len--)
    {
        crc ^= *pcBlock++;

        for (i = 0; i < 8; i++)
            crc = crc & 0x80 ? (crc << 1) ^ 0x13 : crc << 1;
    }

    return crc;
}
