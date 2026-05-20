#define UART_BASE 0x10000000

// UART registers
#define UART_THR  (*(volatile unsigned char *)(UART_BASE + 0))  // transmit
#define UART_LSR  (*(volatile unsigned char *)(UART_BASE + 5))  // line status
#define UART_LSR_EMPTY 0x20                                     // transmit buffer empty

void uart_putc(char c) {
    while ((UART_LSR & UART_LSR_EMPTY) == 0);  // wait until ready
    UART_THR = c;
}

void uart_puts(const char *s) {
    while (*s) uart_putc(*s++);
}
