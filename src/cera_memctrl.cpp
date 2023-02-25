#include <stdlib.h> // malloc()
#include <string.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <linux/kvm.h>

#include <vector>
#include <stdexcept>

#include "./cera_vmem.hpp"
#include "./cera_init.hpp"
#include "./cera_types.hpp"
#include "./cera_memctrl.hpp"
#include "./exceptions/common_exceptions.hpp"

namespace Ceramium {
    Cera_MemCtl::Cera_MemCtl(VM_FD_t VM_FHandle) {
        this->VM_FHandle = VM_FHandle;
    }

    Cera_MemCtl::~Cera_MemCtl() {
        VMem *ci;
        for (size_t i = 0; i < VMem_List.size(); i++) {
            ci = &VMem_List.at(i);
            if (ci->Flags & 0x01) {
                munmap(ci->Host_Memory.Address, ci->Host_Memory.Size);
            }
        }
    }


    void Cera_MemCtl::Create_Mem(Mem_Slot_t V_Slot, size_t N_Pages, off_t VOffset) {
        if (N_Pages == 0) {
            throw std::invalid_argument("N_Pages = 0 is not allowed");
        }

        if (Is_Slot_Taken(V_Slot)) {
            throw slot_taken_error();
        }

        HMem_Area_Specifier hmem = Make_Unbound_VMem(N_Pages);

        _Add_Mem_To_List(V_Slot, &hmem, VOffset, 0x01);
    }

    void Cera_MemCtl::Delete_Mem(Mem_Slot_t V_Slot) {
        VMem *ref;
        int lim = VMem_List.size();
        for (int i = 0; i < lim; i++) {
            ref = &VMem_List.at(i);
            if (ref->VSlot == V_Slot) {
                Detach_HMem(ref->VSlot);

                Delete_VMem(ref->Host_Memory);
                VMem_List.erase(VMem_List.begin()+i);
                return;
            }
        }

        throw std::out_of_range("No module in given slot");
    }

    void Cera_MemCtl::Insert_HMem(Mem_Slot_t V_Slot, HMem_Area_Specifier *Host_Memory, off_t VOffset) {
        if (Is_Slot_Taken(V_Slot)) {
            throw er; // TODO MAKE EXC TYPE
        }

        _Add_Mem_To_List(V_Slot, Host_Memory, VOffset, 0x00);
    }

    void Cera_MemCtl::Detach_HMem(Mem_Slot_t V_Slot) {
        struct kvm_userspace_memory_region reg = {
            .slot = V_Slot,
            .memory_size = 0
        };

        ioctl(VM_FHandle, KVM_SET_USER_MEMORY_REGION, &reg);
    }

    void Cera_MemCtl::Copy_To_Mem(Mem_Slot_t V_Slot, void *H_Address, size_t Length, off_t Offset) {
        for (auto &i : VMem_List) {
            if (i.VSlot == V_Slot) {
                memcpy((void *) (i.Host_Memory.Address + Offset), H_Address, __min(Length, i.Host_Memory.Size - Offset));
            }
        }
    }

    bool Cera_MemCtl::Is_Slot_Taken(Mem_Slot_t V_Slot) {
        if (VMem_List.size() == 0) {
            return false;
        }

        for (auto &i : VMem_List) {
            if (i.VSlot == V_Slot) {
                return true;
            }
        }

        return false;
    }

    void Cera_MemCtl::_Add_Mem_To_List(Mem_Slot_t V_Slot, HMem_Area_Specifier *Host_Memory, off_t VOffset, Mem_Flags_t Flags) {
        struct kvm_userspace_memory_region region = {
            .slot = V_Slot,
            .guest_phys_addr = VOffset,
            .memory_size = Host_Memory->Size,
            .userspace_addr = (u_int64_t) Host_Memory->Address
        };

        ioctl(VM_FHandle, KVM_SET_USER_MEMORY_REGION, &region);

        VMem_List.push_back((VMem) {
            .Host_Memory = *Host_Memory,
            .VSlot = V_Slot,
            .VOffset = VOffset,
            .Flags = Flags
        });
    }
}
