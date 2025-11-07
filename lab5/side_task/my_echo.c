#include <stdio.h>



void my_printf(const char *buffer, unsigned long int size) {
    // This inline assembly ≈ write(1, buffer, size);
    __asm__ volatile (
        "mov $1, %%rax\n\t"      // Load system call number 1 (write) into RAX
        "mov $1, %%rdi\n\t"     // File descriptor: stdout (1)
        "mov %0, %%rsi\n\t"     // Buffer pointer (first input operand)
        "mov %1, %%rdx\n\t"     // Size (second input operand)
        "syscall"                // Invoke the kernel system call to switch from user space to kernel space
        : 
        : "r" (buffer), "r" (size)
        : "rax", "rdi", "rsi", "rdx", "rcx", "r11"
    );
}
    

unsigned int my_strlen(const char*str){
 unsigned int len=0;
 while(str[len]!='\0'){
  len++;

 }
 return len;

}

// Echo-like function using my_printf
void my_echo(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (i > 1) {
            my_printf(" ", 1);  // Space between arguments
        }
        my_printf(argv[i], my_strlen(argv[i]));
    }
    my_printf("\n", 1);  // Newline at end
}


int main(int argc, char *argv[]) {
    my_echo(argc, argv);
    return 0;
}