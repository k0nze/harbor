#include <catch2/catch_test_macros.hpp>

#include "rviss/core/decoder.h"
#include "rviss/rviss.h"

namespace {

uint16_t encode_cr_type(uint16_t bit12, Register rd_rs1, Register rs2) {
    return static_cast<uint16_t>((0b100U << 13U)
        | ((bit12 & 0x1U) << 12U)
        | ((static_cast<uint16_t>(rd_rs1) & 0x1fU) << 7U)
        | ((static_cast<uint16_t>(rs2) & 0x1fU) << 2U)
        | 0b10U);
}

}

TEST_CASE("Decoder decodes C_JR instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cr_type(0U, Register::ra, Register::zero));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_JR*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_JR);
    REQUIRE(compressed->rd_rs1 == Register::ra);
    REQUIRE(compressed->rs2 == Register::zero);
}

TEST_CASE("Decoder decodes C_MV instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cr_type(0U, Register::a0, Register::a1));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_MV*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_MV);
    REQUIRE(compressed->rd_rs1 == Register::a0);
    REQUIRE(compressed->rs2 == Register::a1);
}

TEST_CASE("Decoder decodes C_EBREAK instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cr_type(1U, Register::zero, Register::zero));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_EBREAK*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_EBREAK);
    REQUIRE(compressed->rd_rs1 == Register::zero);
    REQUIRE(compressed->rs2 == Register::zero);
}

TEST_CASE("Decoder decodes C_JALR instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cr_type(1U, Register::s0, Register::zero));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_JALR*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_JALR);
    REQUIRE(compressed->rd_rs1 == Register::s0);
    REQUIRE(compressed->rs2 == Register::zero);
}

TEST_CASE("Decoder decodes C_ADD instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cr_type(1U, Register::s1, Register::s2));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_ADD*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->opcode == Opcode::C_ADD);
    REQUIRE(compressed->rd_rs1 == Register::s1);
    REQUIRE(compressed->rs2 == Register::s2);
}

TEST_CASE("Decoder returns nullptr for reserved C.R encodings with zero rd_rs1 and zero rs2") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cr_type(0U, Register::zero, Register::zero));

    REQUIRE(instruction == nullptr);
}

TEST_CASE("Decoder decodes C.R instructions with the highest register numbers") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_cr_type(1U, Register::t6, Register::t6));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_ADD*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rd_rs1 == Register::t6);
    REQUIRE(compressed->rs2 == Register::t6);
}

TEST_CASE("Decoder only decodes the first compressed C.R instruction from a 32-bit word") {
    Decoder decoder;
    const uint32_t first = encode_cr_type(0U, Register::a2, Register::a3);
    const uint32_t second = encode_cr_type(1U, Register::a4, Register::zero);
    const auto instruction = decoder.decode(first | (second << 16U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_MV*>(instruction.get());
    REQUIRE(compressed != nullptr);
    REQUIRE(compressed->rd_rs1 == Register::a2);
    REQUIRE(compressed->rs2 == Register::a3);
}

TEST_CASE("Decoder does not decode non-CR compressed instructions as C.R") {
    Decoder decoder;
    const auto instruction = decoder.decode(0x0000U);

    REQUIRE(instruction == nullptr);
}

