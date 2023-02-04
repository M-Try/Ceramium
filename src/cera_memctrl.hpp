#pragma once

#include <stdlib.h> // malloc()

#include <sys/ioctl.h>
#include <linux/kvm.h>

#include <stdexcept>

#include "./cera_vmem.hpp"
#include "./cera_init.hpp"

namespace Ceramium {
    class Cera_MemCtl {
    private:
        VM_FD_t VM_FHandle;
        VMem *VMem_List;
        bool *VMem_List_Used_Mask;
        size_t N_VMems;
    public:
        Cera_MemCtl(size_t N_VMems, VM_FD_t VM_FHandle);
        ~Cera_MemCtl();

        void _Resize_HWList(size_t N_VMems);
        VMem_Id_t Insert_HMem(unsigned int V_Slot, HMem_Area_Specifier Host_Memory, off_t VOffset);
    };

    Cera_MemCtl::Cera_MemCtl(size_t N_VMems, VM_FD_t VM_FHandle) {
        if (N_VMems == 0) {
            throw std::invalid_argument("Cannot create Memctl: N_VMems must be at least 1");
        }

        VMem_List = (VMem *) calloc(N_VMems, sizeof(VMem));
        VMem_List_Used_Mask = (bool *) calloc(N_VMems, sizeof(bool));
    }

    void Cera_MemCtl::_Resize_HWList(size_t N_VMems) {
        if (N_VMems == 0) {
            throw std::invalid_argument("Cannot resize HWList: N_VMems must be at least 1");
        }

        VMem_List = (VMem *) realloc(VMem_List, N_VMems * sizeof(VMem));
        VMem_List_Used_Mask = (bool *) realloc(VMem_List_Used_Mask, N_VMems * sizeof(bool));
    }


    VMem_Id_t Cera_MemCtl::Insert_HMem(unsigned int V_Slot, HMem_Area_Specifier Host_Memory, off_t VOffset) {
        for (size_t i = 0; i < N_VMems; i++) {
            if (VMem_List_Used_Mask[i] == false) {
                struct kvm_userspace_memory_region region = {
                    .slot = V_Slot,
                    .guest_phys_addr = VOffset,
                    .memory_size = Host_Memory.Size,
                    .userspace_addr = (u_int64_t) Host_Memory.Address
                };
                
                ioctl(VM_FHandle, KVM_SET_USER_MEMORY_REGION, &region);

                VMem_List_Used_Mask[i] = true;
                VMem_List[i] = {
                    .Id = i,
                    .Host_Memory = Host_Memory,
                    .VOffset = VOffset
                };

                return i;
            }
        }
    }

    Cera_MemCtl::~Cera_MemCtl() {
        free(VMem_List);
    }
}
