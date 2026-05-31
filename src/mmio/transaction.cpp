#include "harbor/mmio/transaction.h"

namespace harbor::mmio {

bool Response::ok() const {
    return status == ResponseStatus::Ok;
}

} // namespace harbor::mmio
