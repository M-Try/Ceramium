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
    Init();

    Cera_Vm _ceratest;

    _ceratest.Insert_Mem(0, 1, 0);
    _ceratest.Set_Mem(0, mem, 0);

    CCore_Id_t _core1 = _ceratest.Insert_Cera_Core();


    _ceratest.Config_Cera_Core(CERA_SETCCRIP, 0, 0x1000);

    _ceratest.Run_Single_Core(_core1, 0);

    _ceratest.Remove_Cera_Core(_core1);
    _ceratest.Remove_Mem(0);

    return 0;
}

/*
while (1) {
    clock_t _begin_time = clock();
            printf("Execution took %f seconds time\n", double(clock() - _begin_time) / CLOCKS_PER_SEC);
        case KVM_EXIT_IO:
            if (run->io.direction == KVM_EXIT_IO_OUT &&
                run->io.size == 1 &&
                run->io.port == 0x3f8 &&
                run->io.count == 1)
*/