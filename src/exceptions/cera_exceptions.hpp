#pragma once

#include <exception>

using std::exception;

class cerainit_error : public exception {
    const char *what() noexcept;
};

class ceravm_construct_error : public exception {
    const char *what() noexcept;
};

class cera_vcpu_entry_error : public exception {
    const char *what() noexcept;
};
