#include "print.h"
#include "uart.h"
#include <stdarg.h>

static void print_uint(unsigned long n, int base) {
    const char *digits = "0123456789abcdef";
    char buf[64];
    int i = 0;

    if (n == 0) {
        uart_putc('0');
        return;
    }

    while (n > 0) {
        buf[i++] = digits[n % base];
        n /= base;
    }

    // digits are in reverse order
    while (i > 0) uart_putc(buf[--i]);
}

static void print_int(long n) {
    if (n < 0) {
        uart_putc('-');
        n = -n;
    }
    print_uint(n, 10);
}

void kprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'c': uart_putc((char)va_arg(args, int));  break;
                case 's': uart_puts(va_arg(args, const char*)); break;
                case 'd': print_int(va_arg(args, int));                 break;
                case 'x': print_uint(va_arg(args, unsigned int), 16);  break;
                case '%': uart_putc('%');                        break;
                default:  uart_putc('%'); uart_putc(*fmt);      break;
            }
        } else {
            uart_putc(*fmt);
        }
        fmt++;
    }

    va_end(args);
}
