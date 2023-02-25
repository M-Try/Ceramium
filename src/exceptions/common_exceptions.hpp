#pragma once

#include <exception>

using std::exception;

class slot_taken_error : public exception {
    const char *what() noexcept;
};
