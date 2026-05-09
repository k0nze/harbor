#include <catch2/catch_test_macros.hpp>

#include "rviss/core/decoder.h"
#include "rviss/rviss.h"

namespace {

uint16_t encode_cb_shift_immediate(uint16_t funct3, uint16_t subop, Register rs1, uint16_t imm) {
    return static_cast<uint16_t>((funct3 << 13U)
        | (((imm >> 5U) & 0x1U) << 12U)
        | ((subop & 0x3U) << 10U)
        | (((static_cast<uint16_t>(rs1) - 8U) & 0x7U) << 7U)
        | ((imm & 0x1fU) << 2U)
        | 0b01U);
}

uint16_t encode_cb_branch_immediate(uint16_t funct3, Register rs1, uint16_t imm) {
    return static_cast<uint16_t>((funct3 << 13U)
        | (((imm >> 8U) & 0x1U) << 12U)
        | ((((imm >> 3U) & 0x3U)) << 10U)
        | (((static_cast<uint16_t>(rs1) - 8U) & 0x7U) << 7U)
        | ((((imm >> 6U) & 0x3U)) << 5U)
        | ((((imm >> 1U) & 0x3U)) << 3U)
        | ((((imm >> 5U) & 0x1U)) << 2U)
        | 0b01U);
}

}

TEST_CASE("Decoder decodes C_SRLI instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cb_shift_immediate(0b100U, 0b00U, Register::s0, 0x21U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_SRLI*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_SRLI);
    REQUIRE(compressed->rs1 == Register::s0);
    REQUIRE(compressed->imm == 0x21U);
}

TEST_CASE("Decoder decodes C_SRAI instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cb_shift_immediate(0b100U, 0b01U, Register::s1, 0x12U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_SRAI*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_SRAI);
    REQUIRE(compressed->rs1 == Register::s1);
    REQUIRE(compressed->imm == 0x12U);
}

TEST_CASE("Decoder decodes C_ANDI instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cb_shift_immediate(0b100U, 0b10U, Register::a0, 0x2aU));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_ANDI*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_ANDI);
    REQUIRE(compressed->rs1 == Register::a0);
    REQUIRE(compressed->imm == 0x2aU);
}

TEST_CASE("Decoder decodes C_BEQZ instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cb_branch_immediate(0b110U, Register::a1, 0x12eU));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_BEQZ*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_BEQZ);
    REQUIRE(compressed->rs1 == Register::a1);
    REQUIRE(compressed->imm == 0x12eU);
}

TEST_CASE("Decoder decodes C_BNEZ instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cb_branch_immediate(0b111U, Register::a2, 0x0d4U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_BNEZ*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_BNEZ);
    REQUIRE(compressed->rs1 == Register::a2);
    REQUIRE(compressed->imm == 0x0d4U);
}

TEST_CASE("Decoder decodes zero C.B shift immediates") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cb_shift_immediate(0b100U, 0b00U, Register::a0, 0x00U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_SRLI*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rs1 == Register::a0);
    REQUIRE(compressed->imm == 0x00U);
}

TEST_CASE("Decoder decodes maximum C.B shift immediates") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cb_shift_immediate(0b100U, 0b01U, Register::a1, 0x3fU));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_SRAI*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rs1 == Register::a1);
    REQUIRE(compressed->imm == 0x3fU);
}

TEST_CASE("Decoder preserves sign-bit-set C_ANDI immediates as encoded bits") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cb_shift_immediate(0b100U, 0b10U, Register::a2, 0x20U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_ANDI*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rs1 == Register::a2);
    REQUIRE(compressed->imm == 0x20U);
}

TEST_CASE("Decoder decodes zero C.B branch immediates") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cb_branch_immediate(0b110U, Register::a3, 0x000U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_BEQZ*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rs1 == Register::a3);
    REQUIRE(compressed->imm == 0x000U);
}

TEST_CASE("Decoder decodes maximum C.B branch immediates") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cb_branch_immediate(0b111U, Register::a4, 0x1feU));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_BNEZ*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rs1 == Register::a4);
    REQUIRE(compressed->imm == 0x1feU);
}

TEST_CASE("Decoder preserves sign-bit-set C.B branch immediates as encoded bits") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cb_branch_immediate(0b110U, Register::a5, 0x100U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_BEQZ*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rs1 == Register::a5);
    REQUIRE(compressed->imm == 0x100U);
}

TEST_CASE("Decoder does not decode non-CB compressed instructions as C.B") {
    Decoder decoder;
    const auto instruction = decoder.decode(0x0000U);

    REQUIRE(instruction == nullptr);
}

TEST_CASE("Decoder routes compressed funct3 100 subop 11 words to C.A decoding") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cb_shift_immediate(0b100U, 0b11U, Register::a0, 0x03U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_SUB*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rd_rs1 == Register::a0);
    REQUIRE(compressed->rs2 == Register::a1);
}

TEST_CASE("Decoder decodes C.B shift instructions with the lowest compact register") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cb_shift_immediate(0b100U, 0b00U, Register::s0, 0x01U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_SRLI*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rs1 == Register::s0);
    REQUIRE(compressed->imm == 0x01U);
}

TEST_CASE("Decoder decodes C.B branch instructions with the highest compact register") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cb_branch_immediate(0b111U, Register::a5, 0x0feU));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_BNEZ*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rs1 == Register::a5);
    REQUIRE(compressed->imm == 0x0feU);
}

TEST_CASE("Decoder only decodes the first compressed C.B instruction from a 32-bit word") {
    Decoder decoder;
    const uint32_t first = encode_cb_shift_immediate(0b100U, 0b10U, Register::a0, 0x15U);
    const uint32_t second = encode_cb_branch_immediate(0b110U, Register::a1, 0x054U);
    const auto instruction = decoder.decode(first | (second << 16U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_ANDI*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rs1 == Register::a0);
    REQUIRE(compressed->imm == 0x15U);
}
