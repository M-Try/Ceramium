// C standard library includes
// no idea what im using these for to be honest
#include <time.h>
#include <fcntl.h> // open()
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <syscall.h>

// platform includes
#include <err.h> // yea so this is actually a bsd extension apparently
#include <sys/mman.h> // mmap() + macros
#include <linux/kvm.h> // obvious
#include <sys/ioctl.h> // ioctl()

// C++ standard library includes
#include <stdexcept>

// local includes
#include "./cera_vm.hpp"
#include "./cera_init.hpp"
#include "./exceptions/cera_exceptions.hpp"


namespace Ceramium {
    Cera_Vm::Cera_Vm(unsigned int VCPU_Count) {
        this->Vm_Descriptor = ioctl(Global_Kvm_Handle, KVM_CREATE_VM, 0);
        if (this->Vm_Descriptor == -1) {
            throw ceravm_construct_error();
        }
        Mem_Ctrl = new Cera_MemCtl(Vm_Descriptor);
        VCores_List_Len = Kvm_VCPU_Id_Max;
        VCores_List = (Cera_VCPU **) calloc(VCores_List_Len, sizeof(Cera_VCPU *));
    }

    Cera_Vm::~Cera_Vm() {
        // safely dispatch virtual machine descriptor
        // TODO: check if this can result in an eternally unbroken loop
        while (1) {
            int _close_ret = close(this->Vm_Descriptor);

            if (_close_ret == 0 || errno != EINTR) {
                break;
            } // otherwise, do not break and continue
        }
    }

    void Cera_Vm::Insert_Mem(Mem_Slot_t V_Slot, size_t N_Pages, off_t VOffset) {
        Mem_Ctrl->Create_Mem(V_Slot, N_Pages, VOffset);
    }

    void Cera_Vm::Add_Host_Mem(Mem_Slot_t V_Slot, HMem_Area_Specifier Host_Mem, off_t VOffset) {
        Mem_Ctrl->Insert_HMem(V_Slot, Host_Mem, VOffset);
    }

    CCore_Id_t Cera_Vm::Insert_Cera_Core(void) {
        for (u_int8_t i = 0; i < VCores_List_Len; i++) {
            if (VCores_List[i] == nullptr) {
                VCores_List[i] = new Cera_VCPU(Vm_Descriptor, i);

                return i;
            }
        }
        throw std::out_of_range("Cannot exceed KVM vCPU limit"); // really awkward to choose this exception type
    }

    void Cera_Vm::Set_Mem(Mem_Slot_t V_Slot, HMem_Area_Specifier Host_Mem_Source, off_t Offset) {
        Mem_Ctrl->Copy_To_Mem(V_Slot, Host_Mem_Source.Address, Host_Mem_Source.Size, Offset);
    }

    void Cera_Vm::Reset_Core(CCore_Id_t Id) {
        if (Id > Kvm_VCPU_Id_Max || VCores_List[Id] == nullptr) {
            throw std::out_of_range("No VCPU with given Core Id");
        }

        reset_vcpu(*VCores_List[Id]);
    }

    void Cera_Vm::FLARE(CC_R_Flags_t Flags) {
        for (u_int8_t i = 0; i < VCores_List_Len; i++) {
            if (VCores_List[i] != nullptr) {
                VCores_List[i]->Run_Threaded();
            }
        }
        return;
    }

    void Cera_Vm::Run_Single_Core(CCore_Id_t Id, CC_R_Flags_t Flags) {
        if (VCores_List[Id] == nullptr || Id > Kvm_VCPU_Id_Max) {
            throw std::out_of_range("Cannot run: No VCPU with given Core Id");
        }

        VCores_List[Id]->Run_Here();
    }

    void Cera_Vm::Remove_Mem(Mem_Slot_t V_Slot) {
        // you could use the Flag in the VMem type to dynamically determine whether
        // to use Delete_Mem or Detach_Mem
        Mem_Ctrl->Delete_Mem(V_Slot);
    }


    void Cera_Vm::Remove_Cera_Core(CCore_Id_t Id) {
        if (Id > Kvm_VCPU_Id_Max || VCores_List[Id] == nullptr) {
            throw std::out_of_range("No VCPU with given Core Id");
        }

        delete VCores_List[Id];
        VCores_List[Id] = nullptr;
    }


}
