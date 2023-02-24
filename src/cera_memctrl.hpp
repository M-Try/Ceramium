#pragma once

#include <sys/types.h>

#include <vector>

#include "./cera_vmem.hpp"
#include "./cera_types.hpp"

namespace Ceramium {
    class Cera_MemCtl {
    private:
        VM_FD_t VM_FHandle;
        std::vector<VMem> VMem_List;
        
        void _Add_Mem_To_List(Mem_Slot_t V_Slot, HMem_Area_Specifier *Host_Memory, off_t VOffset, Mem_Flags_t Flags);
    public:
        Cera_MemCtl(VM_FD_t VM_FHandle);
        ~Cera_MemCtl();

        void Create_Mem(Mem_Slot_t V_Slot, size_t Size, off_t VOffset);
        void Delete_Mem(Mem_Slot_t V_Slot);
        void Insert_HMem(Mem_Slot_t V_Slot, HMem_Area_Specifier *Host_Memory, off_t VOffset);
        void Detach_HMem(Mem_Slot_t V_Slot);

        void Copy_To_Mem(Mem_Slot_t V_Slot, void *H_Address, size_t Length, off_t Offset);
        bool Is_Slot_Taken(Mem_Slot_t V_Slot);
    };
}
