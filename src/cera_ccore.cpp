#include <errno.h>
#include <fcntl.h>
#include <unistd.h> // close()

#include <thread>
#include <stdexcept>

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/kvm.h>

#include "./common.hpp"
#include "./cera_init.hpp"
#include "./cera_ccore.hpp"
#include "./cera_types.hpp"
#include "./exceptions/cera_exceptions.hpp"

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
        // consider that in the event of an error, some cleanup might still be necessary (ie. runhandle acquiring failed but vcpu handle persists and goes stale)
        int vcpufh = ioctl(VM_Handle_, KVM_CREATE_VCPU, New_VCPU_Id);
        if (vcpufh == -1) {
            //if (errno == /*TODO: FIND OUT WHICH ERRNOS THIS THROWS*/) {
                throw std::invalid_argument("cannot make VCPU: VM_Handle_ invalid (EINVAL) or VCPUID exists (EEXIST)");
            //}
        }

        Id = New_VCPU_Id;
        FHandle = vcpufh;
        VM_Handle = VM_Handle_;

        struct kvm_run *runhandle_ = (kvm_run *) mmap(NULL, Kvm_VCPU_Runmap_Size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpufh, 0);

        if (runhandle_ == MAP_FAILED) {
            throw std::runtime_error("Unable to acquire runhandle for vCPU"); // TODO: do better error handling
        }

        Run_Handle = runhandle_;
    }

    void Cera_VCPU::Run_Here(void) {
        while (true) {
            run_vcpu(this->FHandle);

            switch (Run_Handle->exit_reason) {
                case KVM_EXIT_INTERNAL_ERROR:
                case KVM_EXIT_FAIL_ENTRY:
                    throw cera_vcpu_entry_error();
                case KVM_EXIT_HLT:
                    return;
                case KVM_EXIT_IO:
                    if (Run_Handle->io.direction == KVM_EXIT_IO_OUT) {
                        Io_Handlers[Run_Handle->io.port]((((char *)Run_Handle) + Run_Handle->io.data_offset), Run_Handle->io.size);
                    }
                    // if direction is KVM_EXIT_IO_IN, it remains uncaught TODO: fix this
                    break;
                default:
                    //uncaught
                    return;
            }
        }
    }


    void Cera_VCPU::Run_Threaded(void) {
        std::thread T_Core_Run = std::thread(&Ceramium::Cera_VCPU::Run_Here, this);
    }
    
    void Cera_VCPU::Register_IO_Handler(unsigned short IO_Port, Cera_Io_Handler_t Handler) {
        Io_Handlers.insert_or_assign(IO_Port, Handler);
    }

    void Cera_VCPU::Unregister_IO_Handler(unsigned short IO_Port) {
        Io_Handlers.erase(IO_Port);
    }

    void Cera_VCPU::Reset_Core(void) {
        reset_vcpu(this->FHandle);
    }

    CCore_Id_t Cera_VCPU::Get_Core_Id(void) {
        return Id;
    }
}
