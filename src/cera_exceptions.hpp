#pragma once

#include <exception>

class cerainit_error : public std::exception {
    const char *what() noexcept;
};

class ceravm_construct_error : public std::exception {
    const char *what() noexcept;
};

const char *cerainit_error::what() noexcept {
    return "unable to initialize: KVM version 12 required";
}

const char *ceravm_construct_error::what() noexcept {
    return "cannot make VM: run Init() first";
}
