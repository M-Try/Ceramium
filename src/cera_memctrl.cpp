#include <stdlib.h> // malloc()
#include <string.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/kvm.h>


#include <vector>
#include <stdexcept>

#include "./cera_vmem.hpp"
#include "./cera_init.hpp"
#include "./cera_types.hpp"
#include "./cera_memctrl.hpp"

namespace Ceramium {
    Cera_MemCtl::Cera_MemCtl(VM_FD_t VM_FHandle) {
        this->VM_FHandle = VM_FHandle;
    }

    Cera_MemCtl::~Cera_MemCtl() {
        VMem &ci;
        for (size_t i = 0; i < VMem_List.size(); i++) {
            ci = VMem_List.at(i);
            if (ci.Flags & 0x01) {
                munmap(ci.Host_Memory.Address, ci.Host_Memory.Size);
            }
        }
    }


    void Cera_MemCtl::Create_Mem(Mem_Slot_t V_Slot, size_t N_Pages, off_t VOffset) {
        if (N_Pages == 0) {
            throw std::invalid_argument("Cannot create memory module: N_Pages = 0 is not allowed");
        }

        if (Is_Slot_Taken(V_Slot)) {
            throw er; // TODO MAKE EXC TYPE
        }

        void *mem = mmap(NULL, N_Pages * 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if (mem == MAP_FAILED) {
            throw er; // TODO MAKE EXC TYPE
        }

        HMem_Area_Specifier hmem = {
            .Address = mem,
            .Size = N_Pages * 0x1000
        }

        _Add_Mem_To_List(V_Slot, &hmem, VOffset, 0x01);
    }

    void Cera_MemCtl::Delete_Mem(Mem_Slot_t V_Slot) {
        VMem &ref;
        int lim = VMem_List.size();
        for (int i = 0; i < lim; i++) {
            ref = VMem_List.at(i);
            if (ref.VSlot == VSlot) {
                Detach_HMem(ref.VSlot);

                munmap(ref.Host_Memory.Address, ref.Host_Memory.Size);
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
            .memory_size = hmem->Size,
            .userspace_addr = (u_int64_t) hmem->Address
        };

        ioctl(VM_FHandle, KVM_SET_USER_MEMORY_REGION, &region);

        VMem_List.push_back({
            .Host_Memory = *hmem,
            .VSlot = V_Slot,
            .VOffset = VOffset,
            .Flags = Flags
        });
    }
}
