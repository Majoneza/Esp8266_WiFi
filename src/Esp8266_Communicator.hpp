#pragma once

#include <SoftwareSerial.h>
#include <HardwareSerial.h>

// Can be SoftwareSerial or HardwareSerial
using Esp8266_Communicator_Serial = HardwareSerial;

class Esp8266_Communicator : private Esp8266_Communicator_Serial {
private:
    using CSerial = Esp8266_Communicator_Serial;

    bool submitCommand();

    size_t submitAndRead(char* buffer, const size_t size, unsigned long timeout);
public:
    Esp8266_Communicator(const CSerial& serial) : CSerial(serial) {}

    using CSerial::CSerial;

    using CSerial::begin;
    using CSerial::end;

    size_t write(const uint8_t* buffer, const size_t size);

    size_t write(const __FlashStringHelper* str);

    size_t read(char* buffer, const size_t size, unsigned long timeout);

    size_t sendCommand(char* buffer, const size_t size, unsigned long timeout);

    size_t sendCommand(const char* command, char* buffer, const size_t size, unsigned long timeout);

    size_t sendCommand(const __FlashStringHelper* command, char* buffer, const size_t size, unsigned long timeout);
};
