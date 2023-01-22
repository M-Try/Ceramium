/* Ceramium main file
    author: M-Try
    cera reconstruction note: wip

    last edited: 5.9.2022
*/

// C standard library includes
// no idea what im using these for to be honest
#include <time.h>
#include <fcntl.h> // open()
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <syscall.h>
#include <errno.h> // for error no

// platform includes
#include <err.h> // yea so this is actually a bsd extension apparently
#include <sys/mman.h> // for mmap?
#include <linux/kvm.h> // obvious
#include <sys/ioctl.h> // also obvious

// C++ standard library includes
#include <vector>
#include <stdexcept>

// mlib library includes
#include "./include/smlog.hpp"

// local includes
#include "./cera_exceptions.hpp"
#include "./cera.hpp"


namespace Ceramium {
    void Init(void) {
        int kvmfh = open("/dev/kvm", O_RDWR | O_CLOEXEC);
        if(kvmfh == -1) {
            throw cerainit_error();
        }

        int c_check_kvm_ver = ioctl(kvmfh, KVM_GET_API_VERSION, NULL);

        if (c_check_kvm_ver != 12) {
            throw cerainit_error();
        }

        // check if we can use a memory-mapped area as vm memory
        int c_check_kvm_ver = ioctl(kvmfh, KVM_CHECK_EXTENSION, KVM_CAP_USER_MEMORY);
        if (c_check_kvm_ver == -1) {
            throw cerainit_error();
        }

        Kvm_VCPU_Runmap_Size = ioctl(kvmfh, KVM_GET_VCPU_MMAP_SIZE, NULL);

        Global_Kvm_Handle = kvmfh;
    }

    void Release(void) {
        // safely close kvm descriptor
        // TODO: check if this can result in an eternally unbroken loop
        while (1) {
            int _close_ret = close(Global_Kvm_Handle);

            if (_close_ret == 0 || errno != EINTR) {
                break;
            } // otherwise, do not break and continue
        }
    }


    Cera_Vm::Cera_Vm(unsigned int VCPU_Count) {
        this->Vm_Descriptor = ioctl(Global_Kvm_Handle, KVM_CREATE_VM, 0);

        if (this->Vm_Descriptor == -1) {
            throw ceravm_construct_error();
        }
    }
    
    Cera_Vm::~Cera_Vm() {
        // safely dispatch virtual machine descriptor
        // TODO: check if this can result in an eternally unbroken loop
        while (1) {
            int _close_ret = close(this->Vm_Descriptor);

            if (_close_ret == 0 || errno != EINTR) {
                break;
            } // otherwise, do not break and continue
        }
    }
    
    VMem_Id_t Cera_Vm::Insert_Mem(size_t VMem_Size) {
        if (VMem_Size < 1) { // TODO: Make suitable macro for this
            throw er; // TODO: MAKE EXC TYPE OR USE EXISTING ONE -> EVALUATE A FITTING TYPE
        }

        void *mem = mmap(NULL, VMem_Size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); // maybe add support for huge pages (> "HUGETLB")?
        if (mem == MAP_FAILED) {
            throw er; // TODO: MAKE EXC TYPE
        }


        unsigned int memindex = VMem_List.size();

        struct kvm_userspace_memory_region region = {
            .slot = 0,
            .guest_phys_addr = 0x1000,
            .memory_size = 0x1000,
            .userspace_addr = (u_int64_t) mem,
        };

        int _ret = ioctl(vm, KVM_SET_USER_MEMORY_REGION, &region);
        if (_ret < 0) {
            err(EXIT_FAILURE, "IO-control command setting memory region");
            throw er; // TODO: MAKE EXC TYPE
        }
    }

    VMem_Id_t Cera_Vm::Insert_Mem(HMem_Area_Specifier *Mem_Spec, unsigned long long VM_Offset) {
        if (Mem_Spec->Address == nullptr) {
            throw er; // TODO: MAKE EXC TYPE
        }

        void *mem = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if (mem == MAP_FAILED) {
            throw er; // TODO: MAKE EXC TYPE
        }

        struct kvm_userspace_memory_region region = {
            .slot = 0,
            .guest_phys_addr = VM_Offset,
            .memory_size = Mem_Spec->Size,
            .userspace_addr = (u_int64_t) Mem_Spec->Address,
        };

        __int32_t _ret = ioctl(vm, KVM_SET_USER_MEMORY_REGION, &region);
        if (_ret < 0) {
            err(EXIT_FAILURE, "IO-control command setting memory region");
            throw er; // TODO: MAKE EXC TYPE
        }
    }

    CCore_Id_t Cera_Vm::Insert_Cera_Core(void) {
        _newvcpu = acquire_vcpu(this->Vm_Descriptor);
        if (_newvcpu == -1) {
            throw er; // TODO: MAKE EXC TYPE
        }

        acquire_runhandle(_newvcpu);
    }

    void Run_Single_Core(CCore_Id_t Id, CC_R_Flags_t Flags) {
        run_vcpu(Id);
    }

    void Cera_Vm::Remove_Mem(VMem_Id_t Id) {
        /* NOTE / IMPLEMENT / FIXME: be aware of the fact that for Id
            0 < Id < VMem_List.size()
            could be true.
        */
        if (Id < VMem_List.size()) {
            throw er; // TODO: THROW RANGE ERROR OR MAKE SUITABLE EXC TYPE
        }

        try {
            VMem_List.at()
        }
        except (std::out_of_range &e) {

        }
    }








    int acquire_vcpu(int _vm) {
        return ioctl(_vm, KVM_CREATE_VCPU, (unsigned long) 0UL);
    }

    void run_vcpu(int _vcpu) {
        ioctl(_vcpu, KVM_RUN, NULL);
    }

    void reset_vcpu(int _vcpu) {
        struct kvm_sregs sregs;

        ioctl(_vcpu, KVM_GET_SREGS, &sregs); // fetch softregs

        sregs.cs.base = 0;
        sregs.cs.selector = 0;
        
        ioctl(_vcpu, KVM_SET_SREGS, &sregs); // writeback

        struct kvm_regs regs = { // deliberate ad-hoc manipulation for runtime setup
            .rax = 2,
            .rbx = 2,
            .rip = 0x1000,
            .rflags = 0x2,
        };

        ioctl(_vcpu, KVM_SET_REGS, &regs); // write regs
    }

    struct kvm_run *acquire_runhandle(int _kvmsys, int _vcpu) {
        int mmap_size = ioctl(_kvmsys, KVM_GET_VCPU_MMAP_SIZE, NULL);
        struct kvm_run *run = (kvm_run *) mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, _vcpu, 0);

        if (run == MAP_FAILED) {
            err(EXIT_FAILURE, "KVM: mmap kvm_run");
        }
    }
}
