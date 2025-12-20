#include "GPIO.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>


bool GPIO::write_file(const std::string& path,
                      const std::string& data)
{
    int fd = open(path.c_str(), O_WRONLY);
    if (fd < 0)
        return false;

    ssize_t written = write(fd, data.c_str(), data.size());
    close(fd);

    return (written == static_cast<ssize_t>(data.size()));
}

GPIO::GPIO(int bcm, Direction dir)
    : bcm_pin(bcm),
      sysfs_pin(bcm + 512),   // REQUIRED: bcm + 512
      value_fd(-1),
      exported(false)
{
    // 1. Export GPIO
    if (!write_file("/sys/class/gpio/export",
                    std::to_string(sysfs_pin))) {
        return;
    }

    exported = true;
    base_path = "/sys/class/gpio/gpio" + std::to_string(sysfs_pin);

    // 2. Configure direction
    const std::string dir_path = base_path + "/direction";
    if (dir == Direction::In) {
        if (!write_file(dir_path, "in"))
            return;
    } else {
        if (!write_file(dir_path, "out"))
            return;
    }

    // 3. Open value file
    const std::string value_path = base_path + "/value";
    value_fd = open(value_path.c_str(), O_RDWR);
}

GPIO::~GPIO()
{
    release();
}

void GPIO::release()
{
    if (value_fd >= 0) {
        close(value_fd);
        value_fd = -1;
    }

    if (exported) {
        write_file("/sys/class/gpio/unexport",
                   std::to_string(sysfs_pin));
        exported = false;
    }
}

GPIO::GPIO(GPIO&& other) noexcept
    : bcm_pin(other.bcm_pin),
      sysfs_pin(other.sysfs_pin),
      value_fd(other.value_fd),
      exported(other.exported),
      base_path(std::move(other.base_path))
{
    other.value_fd = -1;
    other.exported = false;
}

GPIO& GPIO::operator=(GPIO&& other) noexcept
{
    if (this != &other) {
        release();

        bcm_pin   = other.bcm_pin;
        sysfs_pin = other.sysfs_pin;
        value_fd  = other.value_fd;
        exported  = other.exported;
        base_path = std::move(other.base_path);

        other.value_fd = -1;
        other.exported = false;
    }
    return *this;
}

bool GPIO::write_value(bool value)
{
    if (value_fd < 0)
        return false;

    const char c = value ? '1' : '0';

    // sysfs requires seeking before write
    lseek(value_fd, 0, SEEK_SET);
    ssize_t w = write(value_fd, &c, 1);

    return (w == 1);
}

int GPIO::read_value()
{
    if (value_fd < 0)
        return -1;

    char c;
    lseek(value_fd, 0, SEEK_SET);
    ssize_t r = read(value_fd, &c, 1);

    if (r != 1)
        return -1;

    return (c == '1') ? 1 : 0;
}

bool GPIO::is_valid() const
{
    return exported && value_fd >= 0;
}
