#pragma once

#include <exception>

using std::exception;

class slot_taken_error : public exception {
    public:
    const char *what() noexcept;
};
