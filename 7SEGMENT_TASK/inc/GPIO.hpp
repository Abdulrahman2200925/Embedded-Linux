#ifndef GPIO_HPP
#define GPIO_HPP

#include <string>

class GPIO
{
public:
    enum class Direction
    {
        In,
        Out
    };

    // RAII: acquire GPIO on construction
    GPIO(int bcm_pin, Direction dir);

    // RAII: release GPIO on destruction
    ~GPIO();

    // GPIO must not be copied
    GPIO(const GPIO&) = delete;
    GPIO& operator=(const GPIO&) = delete;

    // GPIO can be moved (transfer ownership)
    GPIO(GPIO&& other) noexcept;
    GPIO& operator=(GPIO&& other) noexcept;

    // Write logic level (for output pins)
    bool write_value(bool value);

    // Read logic level (for input pins)
    // Returns 0 or 1, or -1 on error
    int read_value();

    // Check if GPIO initialized correctly
    bool is_valid() const;

private:
    int bcm_pin;        // BCM pin number
    int sysfs_pin;      // sysfs GPIO number (bcm + 512)
    int value_fd;       // file descriptor for value
    bool exported;      // whether GPIO was successfully exported
    std::string base_path;

private:
    // Helper to write small strings to sysfs files
    static bool write_file(const std::string& path,
                           const std::string& data);

    // Release resources (used by destructor and move assignment)
    void release();
};

#endif // GPIO_HPP
