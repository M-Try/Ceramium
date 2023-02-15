#pragma once

#include <sys/types.h>

#include <vector>

#include "./cera_vmem.hpp"

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
        void Remove_Mem(VMem_Id_t Id);
        void Remove_VMem_At_Slot(unsigned int V_Slot);
        void Free_HMem(VMem_Id_t Id);
    };
}
