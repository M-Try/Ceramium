#include <stddef.h>

// C++ standard library includes
#include <vector>

#include "./cera_vmem.hpp"
#include "./cera_types.hpp"
#include "./cera_ccore.hpp"

using std::vector;

namespace Ceramium {
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
        void Manip_Mem(VMem_Id_t Id, HMem_Area_Specifier Host_Mem_Source);

        void FLARE(CC_R_Flags_t Flags); // full load and run environment
        void Run_Single_Core(CCore_Id_t Id, CC_R_Flags_t Flags);

        void Remove_Mem(VMem_Id_t Id);
        void Remove_Cera_Core(CCore_Id_t Id);
    };
}
