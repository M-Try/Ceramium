#pragma once

#include <stddef.h>
#include <sys/types.h>

#include "./cera_types.hpp"

namespace Ceramium {
    typedef unsigned short Mem_Slot_t;
    typedef unsigned char Mem_Flags_t;

    struct HMem_Area_Specifier {
        void *Address = nullptr;
        size_t Size = 0;
    };

    struct VMem {
        HMem_Area_Specifier Host_Memory;
        Mem_Slot_t VSlot;
        off_t VOffset;
        Mem_Flags_t Flags;
    };

    HMem_Area_Specifier Make_Unbound_VMem(size_t N_Pages);
    void Delete_VMem(HMem_Area_Specifier VMem_Map);
}
