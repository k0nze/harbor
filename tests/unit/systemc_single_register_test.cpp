#include "harbor/systemc/single_register.h"

#include <cassert>
#include <cstdint>

namespace {

void reset_restores_configured_value() {
    harbor::systemc::SingleRegister reg{"reg_reset", 0x12345678U};

    const harbor::mmio::Transaction write{
        .kind = harbor::mmio::AccessKind::Write,
        .address = 0,
        .size = harbor::mmio::AccessSize::Word,
        .write_value = 0xa5a55a5aU,
    };

    assert(reg.transport(write).ok());
    assert(reg.value() == 0xa5a55a5aU);

    reg.reset();
    assert(reg.value() == 0x12345678U);
}

void reads_and_writes_the_register() {
    harbor::systemc::SingleRegister reg{"reg_rw"};

    const harbor::mmio::Transaction write{
        .kind = harbor::mmio::AccessKind::Write,
        .address = 0,
        .size = harbor::mmio::AccessSize::Word,
        .write_value = 0xffffffffa5a55a5aULL,
    };

    const auto write_response = reg.transport(write);
    assert(write_response.ok());
    assert(write_response.read_value == 0xa5a55a5aU);
    assert(reg.value() == 0xa5a55a5aU);

    const harbor::mmio::Transaction read{
        .kind = harbor::mmio::AccessKind::Read,
        .address = 0,
        .size = harbor::mmio::AccessSize::Word,
    };

    const auto read_response = reg.transport(read);
    assert(read_response.ok());
    assert(read_response.read_value == 0xa5a55a5aU);
}

void rejects_invalid_accesses() {
    harbor::systemc::SingleRegister reg{"reg_invalid"};

    const harbor::mmio::Transaction invalid_address{
        .kind = harbor::mmio::AccessKind::Read,
        .address = 4,
        .size = harbor::mmio::AccessSize::Word,
    };

    const auto address_response = reg.transport(invalid_address);
    assert(!address_response.ok());
    assert(address_response.status == harbor::mmio::ResponseStatus::DecodeError);

    const harbor::mmio::Transaction invalid_size{
        .kind = harbor::mmio::AccessKind::Read,
        .address = 0,
        .size = harbor::mmio::AccessSize::Byte,
    };

    const auto size_response = reg.transport(invalid_size);
    assert(!size_response.ok());
    assert(size_response.status == harbor::mmio::ResponseStatus::UnsupportedAccess);
}

} // namespace

int sc_main(int, char *[]) {
    reset_restores_configured_value();
    reads_and_writes_the_register();
    rejects_invalid_accesses();
    return 0;
}
