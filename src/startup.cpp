// Symbols defined by the linker script marking the start/end of .init_array
extern void (*__init_array_start[])();
extern void (*__init_array_end[])();

// Called from boot.S before main() to run global constructors
extern "C" void __init_cpp() {
    for (void (**fn)() = __init_array_start; fn < __init_array_end; fn++) {
        (*fn)();
    }
}
