#include <array>
#include <catch2/catch_test_macros.hpp>

#include "rviss/rviss.h"

namespace {

class MockArchitecturalState : public ArchitecturalState {
public:
    std::array<uint64_t, 32> gprs{};
    uint64_t pc = 0;
    uint64_t last_exception_cause = 0;
    uint64_t last_exception_tval = 0;
    uint64_t last_interrupt_cause = 0;

    uint64_t read_gpr(Register reg) const override {
        return gprs[static_cast<std::size_t>(reg)];
    }

    void write_gpr(Register reg, uint64_t value) override {
        if (reg != Register::zero) {
            gprs[static_cast<std::size_t>(reg)] = value;
        }
    }

    uint64_t read_pc() const override {
        return pc;
    }

    void write_pc(uint64_t value) override {
        pc = value;
    }

    uint16_t fetch16(uint64_t) const override {
        return 0;
    }

    uint32_t fetch32(uint64_t) const override {
        return 0;
    }

    uint8_t load8(uint64_t) const override {
        return 0;
    }

    uint16_t load16(uint64_t) const override {
        return 0;
    }

    uint32_t load32(uint64_t) const override {
        return 0;
    }

    uint64_t load64(uint64_t) const override {
        return 0;
    }

    void store8(uint64_t, uint8_t) override {}

    void store16(uint64_t, uint16_t) override {}

    void store32(uint64_t, uint32_t) override {}

    void store64(uint64_t, uint64_t) override {}

    uint64_t read_csr(uint16_t) const override {
        return 0;
    }

    void write_csr(uint16_t, uint64_t) override {}

    uint32_t load_reserved32(uint64_t) override {
        return 0;
    }

    uint64_t load_reserved64(uint64_t) override {
        return 0;
    }

    bool store_conditional32(uint64_t, uint32_t) override {
        return true;
    }

    bool store_conditional64(uint64_t, uint64_t) override {
        return true;
    }

    void raise_exception(uint64_t cause, uint64_t tval) override {
        last_exception_cause = cause;
        last_exception_tval = tval;
    }

    void raise_interrupt(uint64_t cause) override {
        last_interrupt_cause = cause;
    }

    std::size_t xlen() const override {
        return 64;
    }
};

}

TEST_CASE("ArchitecturalState implementations expose core architectural operations") {
    MockArchitecturalState state;

    state.write_gpr(Register::a0, 0x1234U);
    state.write_gpr(Register::zero, 0xffffU);
    state.write_pc(0x80000000U);
    state.raise_exception(13U, 0xdeadbeefU);
    state.raise_interrupt(5U);

    REQUIRE(state.read_gpr(Register::a0) == 0x1234U);
    REQUIRE(state.read_gpr(Register::zero) == 0U);
    REQUIRE(state.read_pc() == 0x80000000U);
    REQUIRE(state.last_exception_cause == 13U);
    REQUIRE(state.last_exception_tval == 0xdeadbeefU);
    REQUIRE(state.last_interrupt_cause == 5U);
    REQUIRE(state.xlen() == 64U);
}
