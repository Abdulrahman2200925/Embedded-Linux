ENTRY(_start)

SECTIONS
{
    . = 0x10000000;

    .text :
    {
        *(.text.boot)
        *(.text)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(.bss)
    }
}