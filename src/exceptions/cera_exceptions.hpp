#pragma once

#include <exception>

class cerainit_error : public std::exception {
    const char *what() noexcept;
};

class ceravm_construct_error : public std::exception {
    const char *what() noexcept;
};
