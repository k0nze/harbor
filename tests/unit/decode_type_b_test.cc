#include <catch2/catch_test_macros.hpp>

#include "rviss/core/decoder.h"
#include "rviss/rviss.h"

namespace {

uint32_t encode_b_type(uint16_t imm, uint32_t funct3, Register rs1, Register rs2) {
    return (((static_cast<uint32_t>(imm) >> 12U) & 0x1U) << 31U)
        | (((static_cast<uint32_t>(imm) >> 5U) & 0x3fU) << 25U)
        | ((static_cast<uint32_t>(rs2) & 0x1fU) << 20U)
        | ((static_cast<uint32_t>(rs1) & 0x1fU) << 15U)
        | ((funct3 & 0x7U) << 12U)
        | (((static_cast<uint32_t>(imm) >> 1U) & 0x0fU) << 8U)
        | (((static_cast<uint32_t>(imm) >> 11U) & 0x1U) << 7U)
        | 0x63U;
}

}

TEST_CASE("Decoder decodes BEQ instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_b_type(0x0d2U, 0b000U, Register::a0, Register::a1));

    REQUIRE(instruction != nullptr);
    const auto* branch = dynamic_cast<const BEQ*>(instruction.get());
    REQUIRE(branch != nullptr);
    REQUIRE(branch->opcode == Opcode::BEQ);
    REQUIRE(branch->rs1 == Register::a0);
    REQUIRE(branch->rs2 == Register::a1);
    REQUIRE(branch->imm == 0x0d2U);
}

TEST_CASE("Decoder decodes BNE instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_b_type(0x144U, 0b001U, Register::a2, Register::a3));

    REQUIRE(instruction != nullptr);
    const auto* branch = dynamic_cast<const BNE*>(instruction.get());
    REQUIRE(branch != nullptr);
    REQUIRE(branch->opcode == Opcode::BNE);
    REQUIRE(branch->rs1 == Register::a2);
    REQUIRE(branch->rs2 == Register::a3);
    REQUIRE(branch->imm == 0x144U);
}

TEST_CASE("Decoder decodes BLT instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_b_type(0x256U, 0b100U, Register::a4, Register::a5));

    REQUIRE(instruction != nullptr);
    const auto* branch = dynamic_cast<const BLT*>(instruction.get());
    REQUIRE(branch != nullptr);
    REQUIRE(branch->opcode == Opcode::BLT);
    REQUIRE(branch->rs1 == Register::a4);
    REQUIRE(branch->rs2 == Register::a5);
    REQUIRE(branch->imm == 0x256U);
}

TEST_CASE("Decoder decodes BGE instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_b_type(0x368U, 0b101U, Register::a6, Register::a7));

    REQUIRE(instruction != nullptr);
    const auto* branch = dynamic_cast<const BGE*>(instruction.get());
    REQUIRE(branch != nullptr);
    REQUIRE(branch->opcode == Opcode::BGE);
    REQUIRE(branch->rs1 == Register::a6);
    REQUIRE(branch->rs2 == Register::a7);
    REQUIRE(branch->imm == 0x368U);
}

TEST_CASE("Decoder decodes BLTU instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_b_type(0x47aU, 0b110U, Register::s0, Register::s1));

    REQUIRE(instruction != nullptr);
    const auto* branch = dynamic_cast<const BLTU*>(instruction.get());
    REQUIRE(branch != nullptr);
    REQUIRE(branch->opcode == Opcode::BLTU);
    REQUIRE(branch->rs1 == Register::s0);
    REQUIRE(branch->rs2 == Register::s1);
    REQUIRE(branch->imm == 0x047aU);
}

TEST_CASE("Decoder decodes BGEU instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_b_type(0x58cU, 0b111U, Register::s2, Register::s3));

    REQUIRE(instruction != nullptr);
    const auto* branch = dynamic_cast<const BGEU*>(instruction.get());
    REQUIRE(branch != nullptr);
    REQUIRE(branch->opcode == Opcode::BGEU);
    REQUIRE(branch->rs1 == Register::s2);
    REQUIRE(branch->rs2 == Register::s3);
    REQUIRE(branch->imm == 0x058cU);
}

TEST_CASE("Decoder decodes zero B-type immediates") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_b_type(0x0000U, 0b000U, Register::t0, Register::t1));

    REQUIRE(instruction != nullptr);
    const auto* branch = dynamic_cast<const BEQ*>(instruction.get());
    REQUIRE(branch != nullptr);
    REQUIRE(branch->imm == 0x0000U);
}

TEST_CASE("Decoder decodes maximum positive B-type immediates") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_b_type(0x0ffeU, 0b001U, Register::t2, Register::t3));

    REQUIRE(instruction != nullptr);
    const auto* branch = dynamic_cast<const BNE*>(instruction.get());
    REQUIRE(branch != nullptr);
    REQUIRE(branch->imm == 0x0ffeU);
}

TEST_CASE("Decoder preserves sign-bit-set B-type immediates as encoded bits") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_b_type(0x1ffcU, 0b100U, Register::t4, Register::t5));

    REQUIRE(instruction != nullptr);
    const auto* branch = dynamic_cast<const BLT*>(instruction.get());
    REQUIRE(branch != nullptr);
    REQUIRE(branch->imm == 0x1ffcU);
}

TEST_CASE("Decoder returns nullptr for invalid B-type funct3") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_b_type(0x0120U, 0b010U, Register::a0, Register::a1));

    REQUIRE(instruction == nullptr);
}

TEST_CASE("Decoder does not decode compressed words as B-type instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(0x00002000U);

    REQUIRE(instruction == nullptr);
}
