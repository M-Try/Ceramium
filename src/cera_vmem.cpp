#include <sys/mman.h>

#include "./cera_vmem.hpp"

HMem_Area_Specifier Make_Unbound_VMem(size_t N_Pages) {
    if (N_Pages == 0) {
        throw er; // TODO MAKE EXC TYPE
    }

    mmap()
}
void Delete_VMem(HMem_Area_Specifier VMem_Map) {
    int status = munmap(VMem_Map.Address, VMem_Map.Size);
}
