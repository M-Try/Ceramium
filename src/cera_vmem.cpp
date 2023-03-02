#include <errno.h>

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
            if (errno == ENOMEM) {
                // ENOMEM can be fired by the process's RLIMIT_DATA limit, described in getrlimit(2).
                // exceeding that limit causes the error. TODO: try to increase the limit (configurable behaviour?)
                // instead of throwing an error
                throw std::runtime_error("Cannot allocate memory: out of memory");
            }
            else if (errno == EAGAIN) {
                throw std::runtime_error("Cannot allocate memory: too much memory has been locked");
            }
            else {
                throw std::runtime_error("Failed to allocate memory");
            }
        }
        new_map.Size = N_Pages * 0x1000;

        return new_map;
    }

    void Delete_VMem(HMem_Area_Specifier VMem_Map) {
        if (munmap(VMem_Map.Address, VMem_Map.Size) == -1) {
            // likely caused an invalid argument
            // errno contains further information
            throw std::invalid_argument("Specified memory area is not valid");
        }
    }
}
