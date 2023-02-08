#pragma once

#include <stddef.h>
#include <sys/types.h>

#include "./cera_types.hpp"

struct HMem_Area_Specifier {
    void *Address = nullptr;
    size_t Size = 0;
};

struct VMem {
    VMem_Id_t Id;
    HMem_Area_Specifier Host_Memory;
    unsigned int VSlot;
    off_t VOffset;
};
