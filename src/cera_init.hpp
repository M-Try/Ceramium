#pragma once

#include <sys/types.h>

namespace Ceramium {
    inline int Global_Kvm_Handle;
    inline int Kvm_VCPU_Id_Max;
    inline size_t Kvm_VCPU_Runmap_Size;

    void Init(void);
    void Init(char *Kvm_Path);
    void Release(void);
}