#include "/src/cera_init.hpp"

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


int main(int argc, const char **argv) {
    Ceramium::Init();

    Ceramium::Cera_Vm _ceratest;

    _ceratest.Insert_Mem(0, 1);
    _ceratest.Insert_Cera_Core();
    _ceratest.Config_Cera_Core(CERA_SETCCMEM, 0, 0);

    _ceratest.Manip_Mem(0, &vm_guest_code, sizeof(vm_guest_code));

    _ceratest.Config_Cera_Core(CERA_SETCCRIP, 0, 0x1000);

    _ceratest.Run_Single_Core(0, CERA_R_IOLESS);

    _ceratest.Remove_Cera_Core(0);
    _ceratest.Remove_Mem(0);

    return 0;
}

/*
while (1) {
    clock_t _begin_time = clock();

    run_vcpu(vcpu);

    switch (run->exit_reason) {
        case KVM_EXIT_HLT:
            printf("Execution took %f seconds time\n", double(clock() - _begin_time) / CLOCKS_PER_SEC);
            puts("KVM_EXIT_HLT");
                return EXIT_SUCCESS;

        case KVM_EXIT_IO:
            if (run->io.direction == KVM_EXIT_IO_OUT &&
                run->io.size == 1 &&
                run->io.port == 0x3f8 &&
                run->io.count == 1
            ) {
                putchar(*(((char *)run) + run->io.data_offset));
            }
            else {
                errx(1, "unhandled KVM_EXIT_IO");
            }
            break;

        case KVM_EXIT_FAIL_ENTRY:
            logger.log(LOG_FATAL, "hardware entry failure; cannot proceed, exiting.");
            errx(1, "KVM_EXIT_FAIL_ENTRY: hardware_entry_failure_reason = 0x%llx", (unsigned long long) run->fail_entry.hardware_entry_failure_reason);

        case KVM_EXIT_INTERNAL_ERROR:
            logger.log(LOG_FATAL, "KVM kernel internal fault; cannot proceed, exiting.");
            errx(1, "KVM_EXIT_INTERNAL_ERROR: suberror = 0x%x",
            run->internal.suberror);

        default:
            err(EXIT_FAILURE, "what.");
    }
}
*/