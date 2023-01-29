#pragma once

#include <stdlib.h> // malloc()

#include <stdexcept>

#include "./cera_vmem.hpp"
#include "./cera_init.hpp"

namespace Ceramium {
    class Cera_MemCtl {
    private:
        VMem *VMem_List;
    public:
        Cera_MemCtl(unsigned short N_VMems);
        ~Cera_MemCtl();
    };

    Cera_MemCtl::Cera_MemCtl(unsigned short N_VMems) {
        if (N_VMems == 0) {
            throw std::invalid_argument("Cannot create a VM with 0 memory modules");
        }

        VMem_List = (VMem *) calloc(N_VMems, sizeof(VMem));
    }

    Cera_MemCtl::~Cera_MemCtl() {
        free(VMem_List);
    }
}