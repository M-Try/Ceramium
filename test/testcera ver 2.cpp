
    // allocate memory and pass on to guest protomachine
    void *mem = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
        err(EXIT_FAILURE, "mmap: mapping guest memory");
    }

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