#include "count.hpp"

extern "C" {
__attribute__((no_caller_saved_registers, no_instrument_function)) //
void __fentry__() {
    mine->count++;
}
}
