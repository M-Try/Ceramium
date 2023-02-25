#include "./common_exceptions.hpp"

const char *slot_taken_error::what() noexcept {
    return "Memory slot is already taken";
}