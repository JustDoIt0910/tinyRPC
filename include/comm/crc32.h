//
// Created by just do it on 2023/12/2.
//

#ifndef TINYRPC_CRC32_H
#define TINYRPC_CRC32_H
#include <stdint.h>

uint32_t crc32(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint32_t crc = 0xffffffff;        // Initial value
    while(length--)
    {
        crc ^= *data++;                // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;// 0xEDB88320= reverse 0x04C11DB7
            else
                crc = (crc >> 1);
        }
    }
    return ~crc;
}

#endif //TINYRPC_CRC32_H
