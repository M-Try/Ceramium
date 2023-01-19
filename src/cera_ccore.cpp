#include <errno.h>
#include <sys/ioctl.h>

#include "./cera_ccore.hpp"

namespace Ceramium {
    Cera_VCPU::Cera_VCPU(unsigned int VM_Handle_, unsigned int New_VCPU_Id) {
        int vcpufh = ioctl(VM_Handle_, KVM_CREATE_VCPU, New_VCPU_Id);
        if (vcpufh == -1) {
            if (errno == 000 /* FIX THIS; FIND OUT WHICH ERRNOS THIS IOCTL CAN THROW */) {
                throw er; // TODO MAKE EXC TYPE OR THROW EXISTING FITTING ONE
            }
        }
        
        Id = New_VCPU_Id;
        FHandle = vcpufh;
        VM_Handle = VM_Handle_;

        struct kvm_run *runhandle = ioctl(vcpufh, KVM)
    }
}