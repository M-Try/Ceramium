#include <stdio.h>
#include "src/cera_vm.hpp"
#include "src/cera_init.hpp"

const unsigned char vm_guest_code[] = {
    0xba, 0xf8, 0x03, /* mov $0x3f8, %dx ; big endian number*/ 
    0xb0, 'H',        /* mov $'H', %al */
    0xee,             /* out %al, (%dx) */
    0xb0, 'i',        /* mov $'i', %al */
    0xee,             /* out %al, (%dx) */
    0xb0, '!',        /* mov $'!', %al */
    0xee,             /* out %al, (%dx) */
    0xb0, '\n',       /* mov $'\n', %al */
    0xee,             /* out %al, (%dx) */
    0xf4              /* hlt */
};

void io_handler(void *io_area, size_t io_size) {
    putchar(*(char *)io_area);
}

using Ceramium;

HMem_Area_Specifier mem = {&vm_guest_code, sizeof(vm_guest_code)};

int main(int argc, const char **argv) {
    // Initialise
    Init();

    Cera_Vm _ceratest;

    // Create memory module size 0x1000 at slot 0 offset 0
    //. and copy the vm_guest_code into it
    _ceratest.Insert_Mem(0, 1, 0);
    _ceratest.Set_Mem(0, mem, 0);

    // Create virtual CPU and register the IO handler function
    CCore_Id_t _core1 = _ceratest.Insert_Cera_Core();
    _ceratest.Register_IO_Handler(_core1, 0x3f8, io_handler);

    // Run
    _ceratest.Run_Single_Core(_core1, 0);

    // Cleanup
    _ceratest.Remove_Cera_Core(_core1);
    _ceratest.Remove_Mem(0);
    Release();

    return 0;
}
