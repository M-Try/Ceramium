#include <errno.h>
#include <fcntl.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/kvm.h>

#include "./cera_init.hpp"
#include "./exceptions/cera_exceptions.hpp"

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
        int c_check_kvm_ummap = ioctl(kvmfh, KVM_CHECK_EXTENSION, KVM_CAP_USER_MEMORY);
        if (c_check_kvm_ummap == -1) {
            throw cerainit_error();
        }

        Kvm_VCPU_Runmap_Size = ioctl(kvmfh, KVM_GET_VCPU_MMAP_SIZE, NULL);
        Kvm_VCPU_Id_Max = ioctl(kvmfh, KVM_CHECK_EXTENSION, KVM_CAP_MAX_VCPU_ID);
        Global_Kvm_Handle = kvmfh; // should always be the last step
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
}
