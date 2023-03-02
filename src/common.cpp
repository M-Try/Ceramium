#include <stddef.h>

// these are all platform dependendent (linux only/no mingw)
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/kvm.h>

#include "./common.hpp"

namespace Ceramium {
    int acquire_vcpu(int _vm) {
        return ioctl(_vm, KVM_CREATE_VCPU, (unsigned long) 0UL);
    }

    void run_vcpu(int _vcpu) {
        ioctl(_vcpu, KVM_RUN, NULL);
    }

    void reset_vcpu(int _vcpu) {
        struct kvm_sregs sregs;

        ioctl(_vcpu, KVM_GET_SREGS, &sregs); // fetch specregs

        sregs.cs.base = 0; // CRITICAL
        sregs.cs.selector = 0; //CRITICAl
        
        ioctl(_vcpu, KVM_SET_SREGS, &sregs); // writeback

        struct kvm_regs regs = { // deliberate ad-hoc manipulation for runtime setup
            .rip = 0x0,
            .rflags = 0x2, // CRITICAL
        };

        ioctl(_vcpu, KVM_SET_REGS, &regs); // write regs
    }

    size_t get_vcpu_mmap_size(int _kvmsys) {
        int mmap_size = ioctl(_kvmsys, KVM_GET_VCPU_MMAP_SIZE, NULL);
        if (mmap_size == -1) {
            return 0;
        }
        return (size_t) mmap_size;
    }

    struct kvm_run *acquire_runhandle(int _vcpu, size_t _mmap_size) {
        struct kvm_run *run = (kvm_run *) mmap(NULL, _mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, _vcpu, 0);
        if (run == MAP_FAILED) {
            return (kvm_run *) nullptr;
        }
        return run;
    }
}
