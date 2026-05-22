CXX = riscv64-unknown-elf-g++
CXXFLAGS = -march=rv64g -mabi=lp64 -mcmodel=medany -ffreestanding -nostdlib \
           -fno-exceptions -fno-rtti -O0 -g

SRCS = src/boot.S src/trap.S src/switch.S src/startup.cpp src/uart.cpp \
       src/print.cpp src/trap.cpp src/pmem.cpp src/vm.cpp src/elf.cpp \
       src/proc.cpp src/syscall.cpp src/main.cpp

# Build user program first, generating src/user_program.h
src/user_program.h:
	$(MAKE) -C user ../src/user_program.h

kernel: src/user_program.h $(SRCS) linker.ld
	$(CXX) $(CXXFLAGS) -T linker.ld $(SRCS) -o kernel.elf

run: kernel
	qemu-system-riscv64 -machine virt -bios none -kernel kernel.elf -nographic

debug: kernel
	qemu-system-riscv64 -machine virt -bios none -kernel kernel.elf -nographic -S -gdb tcp::1234 &
	gdb-multiarch -q kernel.elf -ex "target remote :1234"

clean:
	$(MAKE) -C user clean
	rm -f kernel.elf
