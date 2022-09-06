#pragma once

#include <stddef.h>
#include <string.h>
#include "TypeUtil.hpp"

static void appendComma(char*& buffer, size_t& size)
{
    if (size > 0) {
        *buffer = ',';
        buffer++; size--;
    }
}

template<bool Optional, typename T>
static enable_if_t<!is_unsigned<T>::value> convertArgument(char*& buffer, size_t& size, T arg)
{
    if constexpr (Optional) {
        if (arg != -1) {
            appendComma(buffer, size);
            return;
        }
    }    
    auto c = BufferUtil::copyNumber(buffer, arg, size);
    buffer += c; size -= c;
    appendComma(buffer, size);
}

template<bool Optional, typename T>
static enable_if_t<is_unsigned<T>::value> convertArgument(char*& buffer, size_t& size, T arg)
{
    if constexpr (Optional) {
        if (arg != 0) {
            appendComma(buffer, size);
            return;
        }
    }
    auto c = BufferUtil::copyNumber(buffer, arg, size);
    buffer += c; size -= c;
    appendComma(buffer, size);
}

template<bool Optional>
void convertArgument(char*& buffer, size_t& size, const char* arg)
{
    if constexpr (Optional) {
        if (arg != nullptr) {
            appendComma(buffer, size);
            return;
        }
    }
    if (size > 0) { *(buffer++) = '\"'; size--; }
    auto c = strlcpy(buffer, arg, size);
    buffer += c; size -= c;
    if (size > 0) { *(buffer++) = '\"'; size--; }
    appendComma(buffer, size);
}

template<>
void convertArgument<false>(char*& buffer, size_t& size, bool arg)
{
    if (size > 0) {
        *buffer = arg ? '1' : '0';
        buffer++; size--;
    }
    appendComma(buffer, size);
}

template<bool Optional, typename... Args>
bool createCommand(char* buffer, size_t size, const char* command, Args... args)
{
    auto c = strlcpy_P(buffer, command, size);
    buffer += c; size -= c;
    (convertArgument<Optional>(buffer, size, args), ...);
    *(--buffer) = '\0';
    return size != 0;
}

template<typename T>
static void parseArgument(char*& buffer, size_t& size, T& arg)
{
    auto c = BufferUtil::readNumber(arg, buffer, size, ',', '\r');
    buffer += c;
    size -= c;
    if (size > 0) {
        buffer++;
        size--;
    }
}

template<>
void parseArgument(char*& buffer, size_t& size, bool& arg)
{
    if (size > 1) {
        arg = *buffer == '1';
        buffer += 2;
        size -= 2;
    }
}

template<>
void parseArgument(char*& buffer, size_t& size, const char*& arg)
{
    arg = buffer;
    auto c = BufferUtil::strnlen(buffer, size, ',', '\r');
    buffer += c;
    size -= c;
    if (size > 0) {
        *(buffer++) = '\0';
        size--;
    }
}

template<typename... Args>
bool parseArguments(char* buffer, size_t size, Args&... args)
{
    (parseArgument(buffer, size, args), ...);
    return size != 0;
}
