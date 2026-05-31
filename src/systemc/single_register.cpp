#include "harbor/systemc/single_register.h"

namespace harbor::systemc {

SingleRegister::SingleRegister(sc_core::sc_module_name name, std::uint32_t reset_value)
    : sc_core::sc_module{name}, reset_value_{reset_value}, value_{reset_value} {}

void SingleRegister::reset() {
    value_ = reset_value_;
}

harbor::mmio::Response SingleRegister::transport(
    const harbor::mmio::Transaction &transaction) {
    if (transaction.address != 0) {
        return {.status = harbor::mmio::ResponseStatus::DecodeError};
    }

    if (transaction.size != harbor::mmio::AccessSize::Word) {
        return {.status = harbor::mmio::ResponseStatus::UnsupportedAccess};
    }

    if (transaction.kind == harbor::mmio::AccessKind::Write) {
        value_ = static_cast<std::uint32_t>(transaction.write_value);
    }

    return {
        .status = harbor::mmio::ResponseStatus::Ok,
        .read_value = value_,
    };
}

std::uint32_t SingleRegister::value() const {
    return value_;
}

} // namespace harbor::systemc
