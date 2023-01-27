#pragma once

#include <stddef.h>

#include <linux/kvm.h>

namespace Ceramium {
    int acquire_vcpu(int _vm);
    void run_vcpu(int _vcpu);
    void reset_vcpu(int _vcpu);
    size_t get_vcpu_mmap_size(int _kvmsys);
    struct kvm_run *acquire_runhandle(int _vcpu, size_t _mmap_size);
}
