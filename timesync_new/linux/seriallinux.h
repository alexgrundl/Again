#ifndef SERIALLINUX_H
#define SERIALLINUX_H

#include "serial.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <string>

class SerialLinux : public Serial
{

public:

    SerialLinux();


    bool Open(uint8_t port);


    bool Open(const char* path);


    bool Close();


    bool SetAttributes(int speed, int parity, bool twoStopBits);


    bool ReadChar(char* buffer, uint32_t bufferPos);


    bool Write(const uint8_t *buffer, uint32_t bytesToWrite);


private:

    int m_fd;


    std::string m_filePath;
};

#endif // SERIALLINUX_H
