//
// Created by just do it on 2023/12/2.
//
#include <iostream>
#include "comm/crc32.h"

int main() {
    uint32_t crc = crc32((uint8_t *) "Hello world", 11);
    std::cout << std::hex << crc << std::endl;

    crc = crc32((uint8_t *) "Hello World", 11);
    std::cout << std::hex << crc << std::endl;
}