#include <catch2/catch_test_macros.hpp>

#include "rviss/core/decoder.h"
#include "rviss/rviss.h"

namespace {

uint16_t encode_css_swsp(Register rs2, uint16_t imm) {
    return static_cast<uint16_t>((0b110U << 13U)
        | (((imm >> 6U) & 0x3U) << 7U)
        | (((imm >> 2U) & 0xfU) << 9U)
        | ((static_cast<uint16_t>(rs2) & 0x1fU) << 2U)
        | 0b10U);
}

uint16_t encode_css_sdsp(Register rs2, uint16_t imm) {
    return static_cast<uint16_t>((0b111U << 13U)
        | (((imm >> 6U) & 0x7U) << 7U)
        | (((imm >> 3U) & 0x7U) << 10U)
        | ((static_cast<uint16_t>(rs2) & 0x1fU) << 2U)
        | 0b10U);
}

}

TEST_CASE("Decoder decodes C_SWSP instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_css_swsp(Register::a0, 0x5cU));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_SWSP*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_SWSP);
    REQUIRE(compressed->rs2 == Register::a0);
    REQUIRE(compressed->imm == 0x5cU);
}

TEST_CASE("Decoder decodes C_SDSP instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_css_sdsp(Register::a1, 0xd8U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_SDSP*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_SDSP);
    REQUIRE(compressed->rs2 == Register::a1);
    REQUIRE(compressed->imm == 0xd8U);
}

TEST_CASE("Decoder decodes zero C.SWSP immediates") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_css_swsp(Register::a2, 0x00U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_SWSP*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->imm == 0x00U);
}

TEST_CASE("Decoder decodes maximum C.SWSP immediates") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_css_swsp(Register::a3, 0xfcU));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_SWSP*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->imm == 0xfcU);
}

TEST_CASE("Decoder decodes maximum C.SDSP immediates") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_css_sdsp(Register::a4, 0x1f8U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_SDSP*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->imm == 0x1f8U);
}

TEST_CASE("Decoder decodes C.SSP instructions with the highest register numbers") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_css_sdsp(Register::t6, 0x40U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_SDSP*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rs2 == Register::t6);
}

TEST_CASE("Decoder only decodes the first compressed C.SSP instruction from a 32-bit word") {
    Decoder decoder;
    const uint32_t first = encode_css_swsp(Register::a0, 0x14U);
    const uint32_t second = encode_css_sdsp(Register::a1, 0x80U);
    const auto instruction = decoder.decode(first | (second << 16U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_SWSP*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rs2 == Register::a0);
    REQUIRE(compressed->imm == 0x14U);
}

TEST_CASE("Decoder does not decode non-CSS compressed instructions as C.SSP") {
    Decoder decoder;
    const auto instruction = decoder.decode(0x2000U);

    REQUIRE(instruction == nullptr);
}
