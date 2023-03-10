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
        int Vm_Descriptor;
        Cera_VCPU **VCores_List;
        size_t VCores_List_Len;
        Cera_MemCtl *Mem_Ctrl;

    public:
        Cera_Vm();
        ~Cera_Vm();

        void Insert_Mem(Mem_Slot_t V_Slot, size_t N_Pages, off_t VOffset);
        void Add_Host_Mem(Mem_Slot_t V_Slot, HMem_Area_Specifier Host_Mem, off_t VOffset);
        CCore_Id_t Insert_Cera_Core(void);

        // bool Config_Cera_Core(CCC_t Config_Code, CCore_Id_t Id, int64_t Arg);
        void Set_Mem(Mem_Slot_t V_Slot, HMem_Area_Specifier Host_Mem_Source, off_t Offset);
        void Register_IO_Handler(CCore_Id_t Id, unsigned short Port, Cera_Io_Handler_t Handler);
        void Unregister_IO_Handler(CCore_Id_t Id, unsigned short Port);
        void Reset_Core(CCore_Id_t Id);
        void FLARE(CC_R_Flags_t Flags); // full load and run environment
        void Run_Single_Core(CCore_Id_t Id, CC_R_Flags_t Flags);

        void Remove_Mem(Mem_Slot_t V_Slot);
        void Remove_Cera_Core(CCore_Id_t Id);
    };
}
