#include "./src/cera.cpp"

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

    _ceratest.Insert_Mem();
    _ceratest.Insert_Cera_Core();
    _ceratest.Config_Cera_Core(CERA_SETCCMEM, 0, 0);

    _ceratest.Manip_Mem(0, (unsigned char *) &vm_guest_code, sizeof(vm_guest_code));

    _ceratest.Config_Cera_Core(CERA_SETCCRIP, 0, 0x1000);


    _ceratest.Run_Single_Core(0, CERA_R_IOLESS);

    _ceratest.Remove_Cera_Core(0);
    _ceratest.Remove_Mem(0);

    memcpy(mem, &vm_guest_code, sizeof(vm_guest_code));

    struct kvm_userspace_memory_region region = {
        .slot = 0,
        .guest_phys_addr = 0x1000,
        .memory_size = 0x1000,
        .userspace_addr = (u_int64_t) mem,
    };

    __int32_t _ret = ioctl(vm, KVM_SET_USER_MEMORY_REGION, &region);
    if (_ret < 0) {
        err(EXIT_FAILURE, "IO-control command setting memory region");
    }

    // acquire vcpu
    vcpu = acquire_vcpu();
    if (vcpu < 0) {
        err(EXIT_FAILURE, "IO-control command acquiring vCPU");
    }

    logger.log(LOG_INFO, "Acquired vCPU_0");

    // setup vcpu



    struct kvm_run *run = acquire_runhandle();


    // custom reset function
    reset_vcpu(vcpu);

    logger.log(LOG_INFO, "vCPU and registers set up, VM ready for execution");

    // run
    while (1) {
        clock_t _begin_time = clock();

        run_vcpu(vcpu);

        switch (run->exit_reason) {
            case KVM_EXIT_HLT:
                printf("Execution took %f seconds time\n", double(clock() - _begin_time) / CLOCKS_PER_SEC);
                logger.log(LOG_INFO, "vCPU halted, execution successful; exiting...");
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

    logger.log(LOG_WARN, "Wh-wh-what? how did i end up near main tail? whatever, exiting.");
    return EXIT_SUCCESS;
}
