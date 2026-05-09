#include <catch2/catch_test_macros.hpp>

#include "rviss/core/decoder.h"
#include "rviss/rviss.h"

namespace {

uint16_t encode_cl_lw(Register rd, Register rs1, uint16_t imm) {
    return static_cast<uint16_t>((0b010U << 13U)
        | (((imm >> 3U) & 0x7U) << 10U)
        | (((static_cast<uint16_t>(rs1) - 8U) & 0x7U) << 7U)
        | (((imm >> 2U) & 0x1U) << 6U)
        | (((imm >> 6U) & 0x1U) << 5U)
        | (((static_cast<uint16_t>(rd) - 8U) & 0x7U) << 2U));
}

uint16_t encode_cl_ld(Register rd, Register rs1, uint16_t imm) {
    return static_cast<uint16_t>((0b011U << 13U)
        | (((imm >> 3U) & 0x7U) << 10U)
        | (((static_cast<uint16_t>(rs1) - 8U) & 0x7U) << 7U)
        | (((imm >> 6U) & 0x3U) << 5U)
        | (((static_cast<uint16_t>(rd) - 8U) & 0x7U) << 2U));
}

}

TEST_CASE("Decoder decodes C_LW instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cl_lw(Register::a0, Register::a1, 0x4cU));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_LW*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_LW);
    REQUIRE(compressed->rd == Register::a0);
    REQUIRE(compressed->rs1 == Register::a1);
    REQUIRE(compressed->imm == 0x4cU);
}

TEST_CASE("Decoder decodes C_LD instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cl_ld(Register::a2, Register::a3, 0xd8U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_LD*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_LD);
    REQUIRE(compressed->rd == Register::a2);
    REQUIRE(compressed->rs1 == Register::a3);
    REQUIRE(compressed->imm == 0xd8U);
}

TEST_CASE("Decoder decodes zero C.LW immediates") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cl_lw(Register::a4, Register::a5, 0x00U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_LW*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->imm == 0x00U);
}

TEST_CASE("Decoder decodes maximum C.LW immediates") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cl_lw(Register::a5, Register::a4, 0x7cU));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_LW*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->imm == 0x7cU);
}

TEST_CASE("Decoder decodes maximum C.LD immediates") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cl_ld(Register::a0, Register::a1, 0xf8U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_LD*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->imm == 0xf8U);
}

TEST_CASE("Decoder decodes C.L instructions with the highest compact registers") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cl_ld(Register::a5, Register::a5, 0x40U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_LD*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rd == Register::a5);
    REQUIRE(compressed->rs1 == Register::a5);
}

TEST_CASE("Decoder only decodes the first compressed C.L instruction from a 32-bit word") {
    Decoder decoder;
    const uint32_t first = encode_cl_lw(Register::a0, Register::a1, 0x18U);
    const uint32_t second = encode_cl_ld(Register::a2, Register::a3, 0x80U);
    const auto instruction = decoder.decode(first | (second << 16U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_LW*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rd == Register::a0);
    REQUIRE(compressed->rs1 == Register::a1);
    REQUIRE(compressed->imm == 0x18U);
}

TEST_CASE("Decoder does not decode non-CL compressed instructions as C.L") {
    Decoder decoder;
    const auto instruction = decoder.decode(0x2000U);

    REQUIRE(instruction == nullptr);
}
