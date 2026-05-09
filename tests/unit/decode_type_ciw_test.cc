#include <catch2/catch_test_macros.hpp>

#include "rviss/core/decoder.h"
#include "rviss/rviss.h"

namespace {

uint16_t encode_ciw_type(Register rd, uint16_t imm) {
    return static_cast<uint16_t>((0b000U << 13U)
        | (((imm >> 4U) & 0x3U) << 11U)
        | (((imm >> 6U) & 0xfU) << 7U)
        | (((imm >> 2U) & 0x1U) << 6U)
        | (((imm >> 3U) & 0x1U) << 5U)
        | (((static_cast<uint16_t>(rd) - 8U) & 0x7U) << 2U));
}

}

TEST_CASE("Decoder decodes C_ADDI4SPN instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ciw_type(Register::a0, 0x154U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_ADDI4SPN*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_ADDI4SPN);
    REQUIRE(compressed->rd == Register::a0);
    REQUIRE(compressed->imm == 0x154U);
}

TEST_CASE("Decoder decodes zero C.IW immediates") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ciw_type(Register::a1, 0x000U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_ADDI4SPN*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rd == Register::a1);
    REQUIRE(compressed->imm == 0x000U);
}

TEST_CASE("Decoder decodes maximum C.IW immediates") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ciw_type(Register::a2, 0x3fcU));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_ADDI4SPN*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rd == Register::a2);
    REQUIRE(compressed->imm == 0x3fcU);
}

TEST_CASE("Decoder decodes C.IW instructions with the highest compact register") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ciw_type(Register::a5, 0x040U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_ADDI4SPN*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rd == Register::a5);
    REQUIRE(compressed->imm == 0x040U);
}

TEST_CASE("Decoder only decodes the first compressed C.IW instruction from a 32-bit word") {
    Decoder decoder;
    const uint32_t first = encode_ciw_type(Register::a0, 0x024U);
    const uint32_t second = encode_ciw_type(Register::a1, 0x1a8U);
    const auto instruction = decoder.decode(first | (second << 16U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_ADDI4SPN*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rd == Register::a0);
    REQUIRE(compressed->imm == 0x024U);
}

TEST_CASE("Decoder does not decode non-CIW compressed instructions as C.IW") {
    Decoder decoder;
    const auto instruction = decoder.decode(0x2000U);

    REQUIRE(instruction == nullptr);
}
