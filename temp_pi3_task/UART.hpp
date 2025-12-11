#ifndef UART_HPP
#define UART_HPP

#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstdio>
#include <cstdint>

class UART {
private:
    int fd;

    static speed_t baud_to_flag(int baud) {
        switch (baud) {
            case 9600:   return B9600;
            case 19200:  return B19200;
            case 38400:  return B38400;
            case 57600:  return B57600;
            case 115200: return B115200;
            default:     return B115200;
        }
    }

public:

    UART(const char *dev = "/dev/serial0", int baud = 115200)
        : fd(-1)
    {
        // Open UART
        fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
        if (fd < 0) {
            perror("UART open");
            return;
        }

        // Set blocking mode
        fcntl(fd, F_SETFL, 0);

        struct termios tty{};
        if (tcgetattr(fd, &tty) != 0) {
            perror("tcgetattr");
            close(fd);
            fd = -1;
            return;
        }

        // Raw mode (no echo, no CRLF conversion, no buffering)
        cfmakeraw(&tty);

        // Baud rate
        speed_t speed = baud_to_flag(baud);
        cfsetispeed(&tty, speed);
        cfsetospeed(&tty, speed);

        // 8N1
        tty.c_cflag |= (CLOCAL | CREAD);    // Enable receiver
        tty.c_cflag &= ~PARENB;             // No parity
        tty.c_cflag &= ~CSTOPB;             // 1 stop bit
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;                 // 8-bit data

        // Blocking: must read at least 1 byte
        tty.c_cc[VMIN]  = 1;
        tty.c_cc[VTIME] = 0;

        // Apply settings
        if (tcsetattr(fd, TCSANOW, &tty) != 0) {
            perror("tcsetattr");
            close(fd);
            fd = -1;
            return;
        }

        tcflush(fd, TCIOFLUSH);  // Clear garbage in buffer
    }

    ~UART() {
        if (fd >= 0) {
            close(fd);
        }
    }

    // Prevent copying
    UART(const UART&) = delete;
    UART& operator=(const UART&) = delete;

    bool ok() const {
        return fd >= 0;
    }

    // Read a single byte (blocking)
    int read_byte() {
        if (fd < 0) return -1;
        uint8_t b;
        int n = read(fd, &b, 1);
        if (n == 1) return b;
        return -1;
    }

    // Read a line until '\n'
    int read_line(char *buf, int maxlen) {
        if (fd < 0) return -1;

        int i = 0;
        while (i < maxlen - 1) {
            int b = read_byte();
            if (b < 0) continue;
            buf[i++] = (char)b;
            if (b == '\n') break;
        }
        buf[i] = '\0';
        return i;
    }

    // Read raw bytes
    int read_bytes(uint8_t *buffer, int size) {
        if (fd < 0) return -1;
        return read(fd, buffer, size);
    }
};

#endif
