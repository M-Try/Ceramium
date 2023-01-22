// these are all platform dependendent (linux only/no mingw)
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/kvm.h>

int acquire_vcpu(int _vm) {
    return ioctl(_vm, KVM_CREATE_VCPU, (unsigned long) 0UL);
}

void run_vcpu(int _vcpu) {
    ioctl(_vcpu, KVM_RUN, NULL);
}

void reset_vcpu(int _vcpu) {
    struct kvm_sregs sregs;

    ioctl(_vcpu, KVM_GET_SREGS, &sregs); // fetch softregs

    sregs.cs.base = 0;
    sregs.cs.selector = 0;
    
    ioctl(_vcpu, KVM_SET_SREGS, &sregs); // writeback

    struct kvm_regs regs = { // deliberate ad-hoc manipulation for runtime setup
        .rax = 2,
        .rbx = 2,
        .rip = 0x1000,
        .rflags = 0x2,
    };

    ioctl(_vcpu, KVM_SET_REGS, &regs); // write regs
}

struct kvm_run *acquire_runhandle(int _kvmsys, int _vcpu) {
    int mmap_size = ioctl(_kvmsys, KVM_GET_VCPU_MMAP_SIZE, NULL);
    struct kvm_run *run = (kvm_run *) mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, _vcpu, 0);

    if (run == MAP_FAILED) {
        err(EXIT_FAILURE, "KVM: mmap kvm_run");
    }
}
