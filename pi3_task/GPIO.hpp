#ifndef GPIO_HPP
#define GPIO_HPP

#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>

class GPIO {
private:
    int bcm_pin;       // BCM pin number
    int sysfs_pin;     // bcm_pin + 512
    int value_fd;      // File descriptor for /value

    bool write_to_file(const std::string &path, const std::string &data) {
        int fd = open(path.c_str(), O_WRONLY);
        if (fd < 0) return false;
        write(fd, data.c_str(), data.size());
        close(fd);
        return true;
    }

public:
    GPIO(int bcm) : bcm_pin(bcm), sysfs_pin(bcm + 512), value_fd(-1) {}

    bool export_pin() {
        return write_to_file("/sys/class/gpio/export", std::to_string(sysfs_pin));
    }

    bool unexport_pin() {
        return write_to_file("/sys/class/gpio/unexport", std::to_string(sysfs_pin));
    }

    bool set_direction(const std::string &direction) {
        std::string path = "/sys/class/gpio/gpio" + std::to_string(sysfs_pin) + "/direction";
        return write_to_file(path, direction);   // "in" or "out"
    }

    bool set_pin(int val) {
        if (value_fd < 0) {
            std::string path = "/sys/class/gpio/gpio" + std::to_string(sysfs_pin) + "/value";
            value_fd = open(path.c_str(), O_WRONLY);
            if (value_fd < 0) return false;
        }
        const char c = (val == 0) ? '0' : '1';
        write(value_fd, &c, 1);
        return true;
    }

    int read_pin() {
        std::string path = "/sys/class/gpio/gpio" + std::to_string(sysfs_pin) + "/value";
        int fd = open(path.c_str(), O_RDONLY);
        if (fd < 0) return -1;
        char buf;
        read(fd, &buf, 1);
        close(fd);
        return (buf == '1') ? 1 : 0;
    }

    ~GPIO() {
        if (value_fd >= 0)
            close(value_fd);
        unexport_pin();
    }
};

#endif
