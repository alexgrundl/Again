#ifndef SERIAL_H
#define SERIAL_H

#include "types.h"

class Serial
{
public:


    virtual bool Open(uint8_t port) = 0;


    virtual bool Open(const char* path) = 0;


    virtual bool Close() = 0;


    virtual bool SetAttributes(int speed, int parity, bool twoStopBits) = 0;


    virtual bool ReadChar(char* buffer, uint32_t bufferPos) = 0;


    virtual bool Write(const uint8_t* buffer, uint32_t bytesToWrite) = 0;
};

#endif // SERIAL_H
