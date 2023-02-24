#pragma once

#include <sys/types.h>

namespace Ceramium {
    int Global_Kvm_Handle;
    int Kvm_VCPU_Id_Max;
    size_t Kvm_VCPU_Runmap_Size;

    void Init(void);
    void Init(char *Kvm_Path);
    void Release(void);
}