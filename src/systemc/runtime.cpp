#include "harbor/systemc/runtime.h"

#include <systemc>

namespace harbor::systemc {

const char *runtime_version() {
    return sc_core::sc_version();
}

} // namespace harbor::systemc
