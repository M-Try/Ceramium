#pragma once

#include <linux/kvm.h>
#include "./cera_types.hpp"

namespace Ceramium {
    class Cera_VCPU {
    private:
        CCore_Id_t FHandle = -1;
        signed int VM_Handle = -1;
        signed int Id = -1;
        struct kvm_run *Run_Handle = nullptr;

    public:
        Cera_VCPU(unsigned int VM_Handle_, unsigned int New_VCPU_Id);
        ~Cera_VCPU();

        void Late_Init(unsigned int VM_Handle_, unsigned int New_VCPU_Id);
        void Run_Here(void);
        void Run_Threaded(void);
        CCore_Id_t Get_Core_Id(void);
    };
}
