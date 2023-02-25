#include <sys/mman.h>

#include <stdexcept>

#include "./cera_vmem.hpp"

namespace Ceramium {
    HMem_Area_Specifier Make_Unbound_VMem(size_t N_Pages) {
        if (N_Pages == 0) {
            throw std::invalid_argument("N_Pages = 0 is not allowed"); // TODO MAKE EXC TYPE
        }

        HMem_Area_Specifier new_map;
        new_map.Address = mmap(NULL, N_Pages * 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if (new_map.Address == MAP_FAILED) {
            throw er; // TODO MAKE EXC TYPE
        }
        new_map.Size = N_Pages * 0x1000;

        return new_map;
    }

    void Delete_VMem(HMem_Area_Specifier VMem_Map) {
        int status = munmap(VMem_Map.Address, VMem_Map.Size);
        if (status == -1) {
            errno;
        }
    }
}
