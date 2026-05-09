#include <catch2/catch_test_macros.hpp>

#include "rviss/core/decoder.h"
#include "rviss/rviss.h"

namespace {

uint16_t encode_cj_type(uint16_t imm) {
    return static_cast<uint16_t>((0b101U << 13U)
        | (((imm >> 11U) & 0x1U) << 12U)
        | (((imm >> 4U) & 0x1U) << 11U)
        | (((imm >> 8U) & 0x3U) << 9U)
        | (((imm >> 10U) & 0x1U) << 8U)
        | (((imm >> 6U) & 0x1U) << 7U)
        | (((imm >> 7U) & 0x1U) << 6U)
        | (((imm >> 1U) & 0x7U) << 3U)
        | (((imm >> 5U) & 0x1U) << 2U)
        | 0b01U);
}

}

TEST_CASE("Decoder decodes C_J instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cj_type(0x4aaU));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_J*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_J);
    REQUIRE(compressed->imm == 0x4aaU);
}

TEST_CASE("Decoder decodes zero C.J immediates") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cj_type(0x000U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_J*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->imm == 0x000U);
}

TEST_CASE("Decoder decodes maximum C.J immediates") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cj_type(0xffeU));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_J*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->imm == 0xffeU);
}

TEST_CASE("Decoder preserves sign-bit-set C.J immediates as encoded bits") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cj_type(0x800U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_J*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->imm == 0x800U);
}

TEST_CASE("Decoder does not decode non-CJ compressed instructions as C.J") {
    Decoder decoder;
    const auto instruction = decoder.decode(0x0000U);

    REQUIRE(instruction == nullptr);
}

TEST_CASE("Decoder only decodes the first compressed C.J instruction from a 32-bit word") {
    Decoder decoder;
    const uint32_t first = encode_cj_type(0x152U);
    const uint32_t second = encode_cj_type(0x6a4U);
    const auto instruction = decoder.decode(first | (second << 16U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_J*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->imm == 0x152U);
}

