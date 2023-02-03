#pragma once

#include <stdlib.h> // malloc()

#include <sys/ioctl.h>
#include <linux/kvm.h>

#include <stdexcept>

#include "./cera_vmem.hpp"
#include "./cera_init.hpp"

namespace Ceramium {
    class Cera_MemCtl {
    private:
        signed int FHandle;
        VMem *VMem_List;
        bool *VMem_List_Used_Mask;
        size_t N_VMems;
    public:
        Cera_MemCtl(size_t N_VMems);
        ~Cera_MemCtl();

        void _Resize_HWList(size_t N_VMems);
        void Insert_Mem(unsigned int V_Slot, VMem New_VMem);
        void Insert_Mem(unsigned int V_Slot, HMem_Area_Specifier Host_Memory, off_t VOffset);
    };

    Cera_MemCtl::Cera_MemCtl(size_t N_VMems) {
        if (N_VMems == 0) {
            throw std::invalid_argument("Cannot create Memctl: N_VMems must be at least 1");
        }

        VMem_List = (VMem *) calloc(N_VMems, sizeof(VMem));
        VMem_List_Used_Mask = (bool *) calloc(N_VMems, sizeof(bool));
    }

    void Cera_MemCtl::_Resize_HWList(size_t N_VMems) {
        if (N_VMems == 0) {
            throw std::invalid_argument("Cannot resize HWList: N_VMems must be at least 1");
        }

        VMem_List = (VMem *) realloc(N_VMems, sizeof(VMem));
        VMem_List_Used_Mask = (bool *) realloc(N_VMems, sizeof(bool));
    }


    VMem_Id_t Cera_MemCtl::Insert_Mem(unsigned int V_Slot, HMem_Area_Specifier Host_Memory, off_t VOffset) {
        for (size_t i = 0; i < N_VMems; i++) {
            if (VMem_List_Used_Mask[i] == false) {
                
            }
        }
    }

    Cera_MemCtl::~Cera_MemCtl() {
        free(VMem_List);
    }
}
