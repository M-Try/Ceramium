#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <thread>

#include "./cera.hpp"
#include "./cera_ccore.hpp"
#include "./cera_types.hpp"

namespace Ceramium {
    Cera_VCPU::Cera_VCPU(unsigned int VM_Handle_, unsigned int New_VCPU_Id) {
        Late_Init(VM_Handle_, New_VCPU_Id);
    }

    Cera_VCPU::~Cera_VCPU() {
        if (Run_Handle != nullptr) {
            munmap((void *) Run_Handle, Kvm_VCPU_Runmap_Size);
        }

        if (FHandle != -1) {
            close(FHandle);
        }
    }

    void Cera_VCPU::Late_Init(unsigned int VM_Handle_, unsigned int New_VCPU_Id) {
        // consider that in the event of an error, some cleanup might still be necessary (ie. runhandle acquiring failed; vcpu handle persists but goes stale)
        int vcpufh = ioctl(VM_Handle_, KVM_CREATE_VCPU, New_VCPU_Id);
        if (vcpufh == -1) {
            if (errno == 000 /* FIX THIS; FIND OUT WHICH ERRNOS THIS IOCTL CAN THROW */) {
                throw er; // TODO MAKE EXC TYPE OR THROW EXISTING FITTING ONE
            }
        }

        Id = New_VCPU_Id;
        FHandle = vcpufh;
        VM_Handle = VM_Handle_;

        struct kvm_run *runhandle_ = (kvm_run *) mmap(NULL, Kvm_VCPU_Runmap_Size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpufh, 0);

        if (runhandle_ == MAP_FAILED) {
            throw er; // TODO MAKE EXC TYPE
        }

        Run_Handle = runhandle_;
    }

    void Cera_VCPU::Run_Here(void) {
        run_vcpu(this->FHandle);

        if (run->exit_reason == KVM_EXIT_FAIL_ENTRY || run->exit_reason == KVM_EXIT_INTERNAL_ERROR) {
            throw er; // TODO MAKE EXC TYPE
        }
    }

    void Cera_VCPU::Run_Threaded(void) {
        std::thread T_Core_Run = std::thread(Ceramium::Cera_VCPU::Run_Here, this);
    }

    CCore_Id_t Cera_VCPU::Get_Core_Id(void) {
        return Id;
    }
}
