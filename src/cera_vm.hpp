#pragma once

#include <stddef.h>

#include <sys/types.h>

#include "./cera_vmem.hpp"
#include "./cera_types.hpp"
#include "./cera_ccore.hpp"
#include "./cera_memctrl.hpp"

namespace Ceramium {
    class Cera_Vm {
    private:
        int Vm_Descriptor = -1;
        Cera_VCPU **VCores_List = nullptr;
        size_t VCores_List_Len;
        Cera_MemCtl *Mem_Ctrl;

    public:
        Cera_Vm(unsigned int VCPU_Count);
        ~Cera_Vm();

        void Insert_Mem(Mem_Slot_t V_Slot, size_t N_Pages, off_t VOffset);
        // void Insert_Mem(Cera_Mem_Initialiser *Initialiser);
        CCore_Id_t Insert_Cera_Core(void);
        // CCore_Id_t Insert_Cera_Core(CC_Initialiser *Initialiser);

        // bool Config_Cera_Core(CCC_t Config_Code, CCore_Id_t Id, int64_t Arg);
        void Set_Mem(Mem_Slot_t V_Slot, HMem_Area_Specifier Host_Mem_Source, off_t Offset);

        void FLARE(CC_R_Flags_t Flags); // full load and run environment
        void Run_Single_Core(CCore_Id_t Id, CC_R_Flags_t Flags);

        void Remove_Mem(Mem_Slot_t V_Slot);
        void Remove_Cera_Core(CCore_Id_t Id);
    };
}
