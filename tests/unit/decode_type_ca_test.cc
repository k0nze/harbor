#include <catch2/catch_test_macros.hpp>

#include "rviss/core/decoder.h"
#include "rviss/rviss.h"

namespace {

uint16_t encode_ca_type(uint16_t bit12, uint16_t funct2, Register rd_rs1, Register rs2) {
    return static_cast<uint16_t>((0b100U << 13U)
        | (bit12 << 12U)
        | (0b11U << 10U)
        | (((static_cast<uint16_t>(rd_rs1) - 8U) & 0x7U) << 7U)
        | ((funct2 & 0x3U) << 5U)
        | (((static_cast<uint16_t>(rs2) - 8U) & 0x7U) << 2U)
        | 0b01U);
}

}

TEST_CASE("Decoder decodes C_SUB instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ca_type(0U, 0b00U, Register::s0, Register::s1));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_SUB*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_SUB);
    REQUIRE(compressed->rd_rs1 == Register::s0);
    REQUIRE(compressed->rs2 == Register::s1);
}

TEST_CASE("Decoder decodes C_XOR instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ca_type(0U, 0b01U, Register::a0, Register::a1));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_XOR*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_XOR);
    REQUIRE(compressed->rd_rs1 == Register::a0);
    REQUIRE(compressed->rs2 == Register::a1);
}

TEST_CASE("Decoder decodes C_OR instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ca_type(0U, 0b10U, Register::a2, Register::a3));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_OR*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_OR);
    REQUIRE(compressed->rd_rs1 == Register::a2);
    REQUIRE(compressed->rs2 == Register::a3);
}

TEST_CASE("Decoder decodes C_AND instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ca_type(0U, 0b11U, Register::a4, Register::a5));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_AND*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_AND);
    REQUIRE(compressed->rd_rs1 == Register::a4);
    REQUIRE(compressed->rs2 == Register::a5);
}

TEST_CASE("Decoder decodes C_SUBW instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ca_type(1U, 0b00U, Register::a4, Register::a5));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_SUBW*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_SUBW);
    REQUIRE(compressed->rd_rs1 == Register::a4);
    REQUIRE(compressed->rs2 == Register::a5);
}

TEST_CASE("Decoder decodes C_ADDW instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ca_type(1U, 0b01U, Register::s0, Register::s1));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_ADDW*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_ADDW);
    REQUIRE(compressed->rd_rs1 == Register::s0);
    REQUIRE(compressed->rs2 == Register::s1);
}

TEST_CASE("Decoder returns nullptr for reserved C.A funct2 combinations") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ca_type(1U, 0b10U, Register::a0, Register::a1));

    REQUIRE(instruction == nullptr);
}

TEST_CASE("Decoder returns nullptr for another reserved C.A funct2 combination") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ca_type(1U, 0b11U, Register::a2, Register::a3));

    REQUIRE(instruction == nullptr);
}

TEST_CASE("Decoder does not decode non-CA compressed instructions as C.A") {
    Decoder decoder;
    const auto instruction = decoder.decode(0x2000U);

    REQUIRE(instruction == nullptr);
}

TEST_CASE("Decoder decodes C.A instructions with the lowest compact registers") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ca_type(0U, 0b00U, Register::s0, Register::s0));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_SUB*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rd_rs1 == Register::s0);
    REQUIRE(compressed->rs2 == Register::s0);
}

TEST_CASE("Decoder decodes C.A instructions with the highest compact registers") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ca_type(0U, 0b11U, Register::a5, Register::a5));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_AND*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rd_rs1 == Register::a5);
    REQUIRE(compressed->rs2 == Register::a5);
}

TEST_CASE("Decoder only decodes the first compressed instruction from a 32-bit word") {
    Decoder decoder;
    const uint32_t first = encode_ca_type(0U, 0b01U, Register::a0, Register::a1);
    const uint32_t second = encode_ca_type(0U, 0b10U, Register::a2, Register::a3);
    const auto instruction = decoder.decode(first | (second << 16U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_XOR*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rd_rs1 == Register::a0);
    REQUIRE(compressed->rs2 == Register::a1);
}
