#ifdef __linux__

#include "seriallinux.h"

SerialLinux::SerialLinux()
{
    m_fd = -1;
}


bool SerialLinux::Open(uint8_t port)
{
    char path[256];
    snprintf(path, sizeof(path), "/dev/ttyS%d", port);
    return Open(path);
}

bool SerialLinux::Open(const char* path)
{
    if(m_fd >= 0)
        Close();

    m_filePath = path;
    m_fd = open (path, O_RDWR | O_NOCTTY | O_SYNC);
    if(m_fd < 0)
        logerror("Couldn't open serial port %s: Errno: %d", path, errno);

    return m_fd >= 0;
}


bool SerialLinux::Close()
{
    if(close(m_fd) == 0)
    {
        m_fd = -1;
        return true;
    }
    return false;
}

bool SerialLinux::SetAttributes(int speed, int parity, bool twoStopBits)
{
    struct termios tty;

    memset (&tty, 0, sizeof tty);
    if (tcgetattr (m_fd, &tty) != 0)
    {
            logerror("Couldn't get attributes of serial port %s: %s", m_filePath.c_str(), strerror(errno));
            return false;
    }

    cfsetospeed (&tty, speed);
    cfsetispeed (&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
                                    // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN]  = 0;            // read doesn't block
    tty.c_cc[VTIME] = 15;            // 1.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL); // shut off xon/xoff ctrl; Do not convert '\r' to '\n'

    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= parity;
    if(!twoStopBits)
        tty.c_cflag &= ~CSTOPB;
    else
        tty.c_cflag |= CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr (m_fd, TCSANOW, &tty) != 0)
    {
        logerror("Couldn't set attributes of serial port %s: Errno: %d", m_filePath.c_str(), errno);
        return false;
    }
    return true;
}

bool SerialLinux::ReadChar(char* buffer, uint32_t bufferPos)
{
    return read(m_fd, buffer + bufferPos, 1) == 1;
}

bool SerialLinux::Write(const uint8_t* buffer, uint32_t bytesToWrite)
{
    uint8_t bytesWritten = 0;
    uint8_t currentBytes = 0;

    while(bytesWritten != bytesToWrite)
    {
        currentBytes = write(m_fd, buffer, bytesToWrite);
        bytesWritten += currentBytes;
        if(currentBytes == 0)
            break;
    }

    return bytesWritten == bytesToWrite;
}

#endif __linux__
