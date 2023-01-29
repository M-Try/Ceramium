#include "./cera_exceptions.hpp"

const char *cerainit_error::what() noexcept {
    return "unable to initialize: KVM version 12 required";
}

const char *ceravm_construct_error::what() noexcept {
    return "cannot make VM: run Init() first";
}

const char *cera_vcpu_entry_error::what() noexcept {
    return "unable to kick VCPU: entry fault";
}
