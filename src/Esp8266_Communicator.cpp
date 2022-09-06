#include "Esp8266_Communicator.hpp"

size_t Esp8266_Communicator::write(const uint8_t* buffer, const size_t size)
{
    size_t c = 0;
    while (c < size) {
        if (CSerial::write(*buffer) == 0) break;
        if (CSerial::timedRead() != *buffer) break;
        buffer++;
        c++;
    }
    return c;
}

size_t Esp8266_Communicator::write(const __FlashStringHelper* str)
{
    PGM_P p = reinterpret_cast<PGM_P>(str);
    size_t c = 0;
    while (1) {
        unsigned char ch = pgm_read_byte(p);
        if (ch == 0) break;
        if (CSerial::write(ch) == 0) break;
        if (CSerial::timedRead() != ch) break;
        p++;
        c++;
    }
    return c;
}

size_t Esp8266_Communicator::read(char* buffer, const size_t size, unsigned long timeout)
{
    // Set new timeout
    auto t = CSerial::getTimeout();
    CSerial::setTimeout(timeout);
    // Read \r\n (maybe remove?)
    if (CSerial::timedRead() != '\r') return 0;
    if (CSerial::timedRead() != '\n') return 0;
    // Read buffer
    auto c = CSerial::readBytes(buffer, size);
    // Unset timeout
    CSerial::setTimeout(t);
    return c;
}

bool Esp8266_Communicator::submitCommand()
{
    if (CSerial::write('\r') == 0) return false;
    if (CSerial::timedRead() != '\r') return false;
    // Newline wont be returned
    if (CSerial::write('\n') == 0) return false;
    return true;
}

size_t Esp8266_Communicator::submitAndRead(char* buffer, const size_t size, unsigned long timeout)
{
    if (submitCommand() == false) return 0;
    return read(buffer, size, timeout);
}

size_t Esp8266_Communicator::sendCommand(char* buffer, const size_t size, unsigned long timeout)
{
    // Write commmand
    auto length = strlen(buffer);
    if (write((uint8_t*)buffer, length) != length) return 0;
    // Submit and read
    return submitAndRead(buffer, size, timeout);
}

size_t Esp8266_Communicator::sendCommand(const char* command, char* buffer, const size_t size, unsigned long timeout)
{
    // Write commmand
    auto length = strlen(command);
    if (write((const uint8_t*)command, length) != length) return 0;
    // Submit and read
    return submitAndRead(buffer, size, timeout);
}

size_t Esp8266_Communicator::sendCommand(const __FlashStringHelper* command, char* buffer, const size_t size, unsigned long timeout)
{
    // Write commmand
    auto w = write(command);
    if (pgm_read_byte(reinterpret_cast<PGM_P>(command) + w) != '\0') return 0;
    // Submit and read
    return submitAndRead(buffer, size, timeout);
}
