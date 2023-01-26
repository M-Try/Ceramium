#include <stddef.h>
//#include <sys/types.h>

// C++ standard library includes
#include <vector>

#include "./cera_vmem.hpp"
#include "./cera_types.hpp"
#include "./cera_ccore.hpp"

using std::vector;

namespace Ceramium {
    int Global_Kvm_Handle;
    int Kvm_VCPU_Id_Max;
    size_t Kvm_VCPU_Runmap_Size;

    void Init(void);
    void Release(void);

    class Cera_Vm {
    private:
        int Vm_Descriptor = -1;
        vector<Cera_VCPU> VCores_List;
        vector<VMem> VMem_List;

    public:
        Cera_Vm(unsigned int VCPU_Count);
        ~Cera_Vm();

        VMem_Id_t Insert_Mem(HMem_Area_Specifier *Mem_Spec);
        VMem_Id_t Insert_Mem(Cera_Mem_Initialiser *Initialiser);
        CCore_Id_t Insert_Cera_Core(void);
        CCore_Id_t Insert_Cera_Core(CC_Initialiser *Initialiser);

        bool Config_Cera_Core(CCC_t Config_Code, CCore_Id_t Id, int64_t Arg);
        void Manip_Mem(VMem_Id_t Id, void *Source, size_t Length);

        void FLARE(CC_R_Flag_t Flags); // full load and run environment
        void Run_Single_Core(CCore_Id_t Id, CC_R_Flag_t Flags);

        void Remove_Mem(VMem_Id_t Id);
        void Remove_Cera_Core(CCore_Id_t Id);
    };

    int acquire_vcpu(int _vm);
    void run_vcpu(int _vcpu);
    void reset_vcpu(int _vcpu);
    struct kvm_run *acquire_runhandle(int _kvmsys, int _vcpu);
}
