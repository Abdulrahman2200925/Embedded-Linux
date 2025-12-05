#include "GPIO.hpp"
#include <unistd.h>

int main() {
    GPIO led(17);  // BCM GPIO 17 = sysfs pin 529

    led.export_pin();

    led.set_direction("out");

    while (true) {
        led.set_pin(1);
        sleep(1);
        led.set_pin(0);
        sleep(1);
    }

    return 0;
}
