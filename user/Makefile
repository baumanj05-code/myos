CC     = riscv64-unknown-elf-gcc
CFLAGS = -march=rv64g -mabi=lp64 -mcmodel=medany -ffreestanding -nostdlib -O0 -g

hello.elf: start.S syscall.S hello.c user.ld
	$(CC) $(CFLAGS) -T user.ld start.S syscall.S hello.c -o hello.elf

# Convert ELF to a C byte array the kernel can include
../src/user_program.h: hello.elf
	xxd -i hello.elf | sed 's/hello_elf/user_program/g; s/unsigned char/const unsigned char/g' > ../src/user_program.h

clean:
	rm -f hello.elf ../src/user_program.h
