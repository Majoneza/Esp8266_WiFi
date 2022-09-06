#pragma once

#include <stddef.h>
#include "TypeUtil.hpp"

namespace BufferUtil {
    template<typename T>
    size_t copyNumber(char* dest, T src, size_t destsize) {
        T n = 1;
        while (src > n) {
            if (destsize == 0) return 0;
            n *= 100;
            destsize--;
        }
        if (n / 10 > src) n /= 10;
        size_t c = 0;
        while (n != 1) {
            n /= 10;
            *dest = (src / n) + '0';
            src %= n;
            dest++;
            c++;
        }
        return c;
    }
    
    template<typename T, typename... Terms>
    size_t readNumber(T& dest, const char* src, size_t srcsize, Terms... terminators)
    {
        dest = 0;
        bool neg = *src == '-';
        size_t c = neg ? 1 : 0;
        while (c < srcsize) {
            if ((... || (src[c] == terminators))) break;
            dest += src[c] - '0';
            dest *= 10; // base = 10
            c++;
        }
        if (neg) dest *= -1;
        return c;
    }

    template<typename... Terms>
    size_t strnlen(const char* str, size_t size, Terms... terminators)
    {
        size_t c = 0;
        while (c < size) {
            if ((... || (str[c] == terminators))) break;
            c++;
        }
        return c;
    }
}
