#include "harbor/mmio/transaction.h"

#include <cassert>
#include <cstdint>

namespace {

class EchoTarget final : public harbor::mmio::Target {
public:
    void reset() override {
        reset_called_ = true;
    }

    harbor::mmio::Response transport(const harbor::mmio::Transaction &transaction) override {
        last_transaction_ = transaction;

        if (transaction.size != harbor::mmio::AccessSize::Word) {
            return {.status = harbor::mmio::ResponseStatus::UnsupportedAccess};
        }

        if (transaction.kind == harbor::mmio::AccessKind::Read) {
            return {
                .status = harbor::mmio::ResponseStatus::Ok,
                .read_value = transaction.address,
                .latency = 1000,
            };
        }

        return {
            .status = harbor::mmio::ResponseStatus::Ok,
            .read_value = transaction.write_value,
            .latency = 2000,
        };
    }

    [[nodiscard]] bool reset_called() const {
        return reset_called_;
    }

    [[nodiscard]] harbor::mmio::Transaction last_transaction() const {
        return last_transaction_;
    }

private:
    bool reset_called_ = false;
    harbor::mmio::Transaction last_transaction_{};
};

void exposes_expected_access_sizes() {
    static_assert(static_cast<std::uint8_t>(harbor::mmio::AccessSize::Byte) == 1U);
    static_assert(static_cast<std::uint8_t>(harbor::mmio::AccessSize::HalfWord) == 2U);
    static_assert(static_cast<std::uint8_t>(harbor::mmio::AccessSize::Word) == 4U);
    static_assert(static_cast<std::uint8_t>(harbor::mmio::AccessSize::DoubleWord) == 8U);
}

void response_reports_success_status() {
    const harbor::mmio::Response ok_response{};
    const harbor::mmio::Response error_response{
        .status = harbor::mmio::ResponseStatus::DecodeError,
    };

    assert(ok_response.ok());
    assert(!error_response.ok());
}

void target_transports_environment_neutral_transactions() {
    EchoTarget target;
    target.reset();
    assert(target.reset_called());

    const harbor::mmio::Transaction read_transaction{
        .kind = harbor::mmio::AccessKind::Read,
        .address = 0x20,
        .size = harbor::mmio::AccessSize::Word,
    };

    const auto read_response = target.transport(read_transaction);
    assert(read_response.ok());
    assert(read_response.read_value == 0x20U);
    assert(read_response.latency == 1000U);
    assert(target.last_transaction().address == 0x20U);

    const harbor::mmio::Transaction write_transaction{
        .kind = harbor::mmio::AccessKind::Write,
        .address = 0x24,
        .size = harbor::mmio::AccessSize::Word,
        .write_value = 0xa5a55a5aU,
    };

    const auto write_response = target.transport(write_transaction);
    assert(write_response.ok());
    assert(write_response.read_value == 0xa5a55a5aU);
    assert(write_response.latency == 2000U);
    assert(target.last_transaction().address == 0x24U);

    const harbor::mmio::Transaction byte_transaction{
        .kind = harbor::mmio::AccessKind::Read,
        .address = 0x01,
        .size = harbor::mmio::AccessSize::Byte,
    };

    const auto byte_response = target.transport(byte_transaction);
    assert(!byte_response.ok());
    assert(byte_response.status == harbor::mmio::ResponseStatus::UnsupportedAccess);
}

} // namespace

int main() {
    exposes_expected_access_sizes();
    response_reports_success_status();
    target_transports_environment_neutral_transactions();
    return 0;
}
