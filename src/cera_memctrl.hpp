#pragma once

#include <stdlib.h> // malloc()

#include <sys/ioctl.h>
#include <linux/kvm.h>

#include <vector>
#include <stdexcept>

#include "./cera_vmem.hpp"
#include "./cera_init.hpp"

namespace Ceramium {
    class Cera_MemCtl {
    private:
        VM_FD_t VM_FHandle;
        VMem_Id_t Next_VMem_Id;
        std::vector<VMem> VMem_List;
    public:
        Cera_MemCtl(size_t N_VMems, VM_FD_t VM_FHandle);
        ~Cera_MemCtl();

        VMem_Id_t Add_Mem(unsigned int V_Slot, size_t Size, off_t VOffset);
        VMem_Id_t Insert_HMem(unsigned int V_Slot, HMem_Area_Specifier Host_Memory, off_t VOffset);
        void Remove_VMem_At_Slot(unsigned int V_Slot);
        void Free_HMem(VMem_Id_t Id);
    };

    Cera_MemCtl::Cera_MemCtl(size_t N_VMems, VM_FD_t VM_FHandle) {
        if (N_VMems == 0) {
            throw std::invalid_argument("Cannot create Memctl: N_VMems must be at least 1");
        }

        this->Next_VMem_Id = 0;
        this->VM_FHandle = VM_FHandle;
    }

    Cera_MemCtl::~Cera_MemCtl() {
        free(VMem_List);
    }

    VMem_Id_t Cera_MemCtl::Add_Mem(unsigned int V_Slot, size_t N_Pages, off_t VOffset) {
        if (N_Pages == 0) {
            throw std::invalid_argument("Cannot create memory module: N_Pages = 0 is not allowed");
        }

        void *mem = mmap(NULL, N_Pages * 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if (mem == MAP_FAILED) {
            throw er; // TODO MAKE EXC TYPE
        }

        struct kvm_userspace_memory_region region = {
            .slot = V_Slot,
            .guest_phys_addr = VOffset,
            .memory_size = N_Pages * 0x1000,
            .userspace_addr = (u_int64_t) mem
        };
        ioctl(VM_FHandle, KVM_SET_USER_MEMORY_REGION, &region); // IMPLEMENTME: Error handling

        VMem_List.push_back({
            .Id = Next_VMem_Id,
            .Host_Memory = Host_Memory,
            .VSlot = V_Slot,
            .VOffset = VOffset
        });

        return Next_VMem_Id++;
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

    void Cera_MemCtl::Remove_VMem_At_Slot(unsigned int V_Slot) {
        struct kvm_userspace_memory_region reg = {
            .slot = V_Slot,
            .memory_size = 0
        };

        ioctl(VM_FHandle, KVM_SET_USER_MEMORY_REGION, &reg);
    }

    void Cera_MemCtl::Free_HMem(VMem_Id_t Id) {
        size_t _size = VMem_List.size();
        for (auto &i : VMem_List) {
            if (i.Id == Id) {
                Remove_VMem_At_Slot(i.VSlot);

                munmap(i.Host_Memory.Address, i.Host_Memory.Size);
                VMem_List.erase(VMem_List.begin()+i);
                return;
            }
        }

        throw std::out_of_range("No VMem with given VMem_Id");
    }
}
