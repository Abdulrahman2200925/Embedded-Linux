#define GPIO_BASE     0x3F200000

#define GPFSEL2  (*(volatile unsigned int *)(GPIO_BASE + 0x08))
#define GPSET0   (*(volatile unsigned int *)(GPIO_BASE + 0x1C))
#define GPCLR0   (*(volatile unsigned int *)(GPIO_BASE + 0x28))

void delay(unsigned long count)
{
    volatile unsigned long i;
    for(i = 0; i < count; i++);
}

void gpio_init(void)
{
    GPFSEL2 &= ~(7 << 18);
    GPFSEL2 |=  (1 << 18);
}

void main(void)
{
    gpio_init();

    while(1)
    {
        GPSET0 = (1 << 26);
         delay(50000000);
        GPCLR0 = (1 << 26);
         delay(50000000);
    }
}