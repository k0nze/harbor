#include <catch2/catch_test_macros.hpp>

#include "rviss/core/decoder.h"
#include "rviss/rviss.h"

namespace {

uint16_t encode_ci_type(uint16_t quadrant, uint16_t funct3, Register rd_rs1, uint16_t imm) {
    return static_cast<uint16_t>((funct3 << 13U)
        | (((imm >> 5U) & 0x1U) << 12U)
        | ((static_cast<uint16_t>(rd_rs1) & 0x1fU) << 7U)
        | ((imm & 0x1fU) << 2U)
        | (quadrant & 0x3U));
}

}

TEST_CASE("Decoder decodes C_NOP instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ci_type(0b01U, 0b000U, Register::zero, 0x00U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_NOP*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_NOP);
    REQUIRE(compressed->rd_rs1 == Register::zero);
    REQUIRE(compressed->imm == 0x00U);
}

TEST_CASE("Decoder decodes C_ADDI instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ci_type(0b01U, 0b000U, Register::a0, 0x15U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_ADDI*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_ADDI);
    REQUIRE(compressed->rd_rs1 == Register::a0);
    REQUIRE(compressed->imm == 0x15U);
}

TEST_CASE("Decoder decodes C_ADDIW instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ci_type(0b01U, 0b001U, Register::a1, 0x21U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_ADDIW*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_ADDIW);
    REQUIRE(compressed->rd_rs1 == Register::a1);
    REQUIRE(compressed->imm == 0x21U);
}

TEST_CASE("Decoder decodes C_LI instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ci_type(0b01U, 0b010U, Register::a2, 0x0bU));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_LI*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_LI);
    REQUIRE(compressed->rd_rs1 == Register::a2);
    REQUIRE(compressed->imm == 0x0bU);
}

TEST_CASE("Decoder decodes C_LUI instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ci_type(0b01U, 0b011U, Register::a3, 0x2cU));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_LUI*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_LUI);
    REQUIRE(compressed->rd_rs1 == Register::a3);
    REQUIRE(compressed->imm == 0x2cU);
}

TEST_CASE("Decoder decodes C_ADDI16SP instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ci_type(0b01U, 0b011U, Register::sp, 0x13U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_ADDI16SP*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_ADDI16SP);
    REQUIRE(compressed->rd_rs1 == Register::sp);
    REQUIRE(compressed->imm == 0x13U);
}

TEST_CASE("Decoder decodes C_SLLI instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ci_type(0b10U, 0b000U, Register::a4, 0x03U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_SLLI*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_SLLI);
    REQUIRE(compressed->rd_rs1 == Register::a4);
    REQUIRE(compressed->imm == 0x03U);
}

TEST_CASE("Decoder decodes C_LWSP instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ci_type(0b10U, 0b010U, Register::a5, 0x14U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_LWSP*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_LWSP);
    REQUIRE(compressed->rd_rs1 == Register::a5);
    REQUIRE(compressed->imm == 0x14U);
}

TEST_CASE("Decoder decodes C_LDSP instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ci_type(0b10U, 0b011U, Register::a6, 0x18U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_LDSP*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_LDSP);
    REQUIRE(compressed->rd_rs1 == Register::a6);
    REQUIRE(compressed->imm == 0x18U);
}

TEST_CASE("Decoder preserves sign-bit-set C.I immediates as encoded bits") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ci_type(0b01U, 0b010U, Register::a7, 0x20U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_LI*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rd_rs1 == Register::a7);
    REQUIRE(compressed->imm == 0x20U);
}

TEST_CASE("Decoder decodes maximum C.I immediates") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_ci_type(0b10U, 0b011U, Register::a0, 0x3fU));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_LDSP*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rd_rs1 == Register::a0);
    REQUIRE(compressed->imm == 0x3fU);
}

TEST_CASE("Decoder does not decode non-CI compressed instructions as C.I") {
    Decoder decoder;
    const auto instruction = decoder.decode(0x0000U);

    REQUIRE(instruction == nullptr);
}

TEST_CASE("Decoder only decodes the first compressed C.I instruction from a 32-bit word") {
    Decoder decoder;
    const uint32_t first = encode_ci_type(0b01U, 0b001U, Register::a1, 0x09U);
    const uint32_t second = encode_ci_type(0b10U, 0b010U, Register::a2, 0x1cU);
    const auto instruction = decoder.decode(first | (second << 16U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_ADDIW*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rd_rs1 == Register::a1);
    REQUIRE(compressed->imm == 0x09U);
}
