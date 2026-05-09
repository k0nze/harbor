#include <catch2/catch_test_macros.hpp>

#include "rviss/core/decoder.h"
#include "rviss/rviss.h"

namespace {

uint32_t encode_r_type(uint32_t opcode, uint32_t funct3, uint32_t funct7, Register rd, Register rs1, Register rs2) {
    return ((funct7 & 0x7fU) << 25U)
        | ((static_cast<uint32_t>(rs2) & 0x1fU) << 20U)
        | ((static_cast<uint32_t>(rs1) & 0x1fU) << 15U)
        | ((funct3 & 0x7U) << 12U)
        | ((static_cast<uint32_t>(rd) & 0x1fU) << 7U)
        | (opcode & 0x7fU);
}

template <typename InstructionT>
void require_r_type_decode(
    uint32_t opcode,
    uint32_t funct3,
    uint32_t funct7,
    Register rd,
    Register rs1,
    Register rs2,
    Opcode expected_opcode
) {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_r_type(opcode, funct3, funct7, rd, rs1, rs2));

    REQUIRE(instruction != nullptr);
    const auto* operation = dynamic_cast<const InstructionT*>(instruction.get());
    REQUIRE(operation != nullptr);
    REQUIRE(operation->opcode == expected_opcode);
    REQUIRE(operation->rd == rd);
    REQUIRE(operation->rs1 == rs1);
    REQUIRE(operation->rs2 == rs2);
}

}

TEST_CASE("Decoder decodes ADD instructions") {
    require_r_type_decode<ADD>(0x33U, 0b000U, 0b0000000U, Register::a0, Register::a1, Register::a2, Opcode::ADD);
}

TEST_CASE("Decoder decodes SUB instructions") {
    require_r_type_decode<SUB>(0x33U, 0b000U, 0b0100000U, Register::a3, Register::a4, Register::a5, Opcode::SUB);
}

TEST_CASE("Decoder decodes SLL instructions") {
    require_r_type_decode<SLL>(0x33U, 0b001U, 0b0000000U, Register::a6, Register::a7, Register::s0, Opcode::SLL);
}

TEST_CASE("Decoder decodes SLT instructions") {
    require_r_type_decode<SLT>(0x33U, 0b010U, 0b0000000U, Register::s1, Register::s2, Register::s3, Opcode::SLT);
}

TEST_CASE("Decoder decodes SLTU instructions") {
    require_r_type_decode<SLTU>(0x33U, 0b011U, 0b0000000U, Register::s4, Register::s5, Register::s6, Opcode::SLTU);
}

TEST_CASE("Decoder decodes XOR instructions") {
    require_r_type_decode<XOR>(0x33U, 0b100U, 0b0000000U, Register::s7, Register::s8, Register::s9, Opcode::XOR);
}

TEST_CASE("Decoder decodes SRL instructions") {
    require_r_type_decode<SRL>(0x33U, 0b101U, 0b0000000U, Register::s10, Register::s11, Register::t0, Opcode::SRL);
}

TEST_CASE("Decoder decodes SRA instructions") {
    require_r_type_decode<SRA>(0x33U, 0b101U, 0b0100000U, Register::t1, Register::t2, Register::t3, Opcode::SRA);
}

TEST_CASE("Decoder decodes OR instructions") {
    require_r_type_decode<OR>(0x33U, 0b110U, 0b0000000U, Register::t4, Register::t5, Register::t6, Opcode::OR);
}

TEST_CASE("Decoder decodes AND instructions") {
    require_r_type_decode<AND>(0x33U, 0b111U, 0b0000000U, Register::ra, Register::sp, Register::gp, Opcode::AND);
}

TEST_CASE("Decoder decodes ADDW instructions") {
    require_r_type_decode<ADDW>(0x3bU, 0b000U, 0b0000000U, Register::tp, Register::t0, Register::t1, Opcode::ADDW);
}

TEST_CASE("Decoder decodes SUBW instructions") {
    require_r_type_decode<SUBW>(0x3bU, 0b000U, 0b0100000U, Register::t2, Register::t3, Register::t4, Opcode::SUBW);
}

TEST_CASE("Decoder decodes SLLW instructions") {
    require_r_type_decode<SLLW>(0x3bU, 0b001U, 0b0000000U, Register::t5, Register::t6, Register::a0, Opcode::SLLW);
}

TEST_CASE("Decoder decodes SRLW instructions") {
    require_r_type_decode<SRLW>(0x3bU, 0b101U, 0b0000000U, Register::a1, Register::a2, Register::a3, Opcode::SRLW);
}

TEST_CASE("Decoder decodes SRAW instructions") {
    require_r_type_decode<SRAW>(0x3bU, 0b101U, 0b0100000U, Register::a4, Register::a5, Register::a6, Opcode::SRAW);
}

TEST_CASE("Decoder decodes MUL instructions") {
    require_r_type_decode<MUL>(0x33U, 0b000U, 0b0000001U, Register::a7, Register::s0, Register::s1, Opcode::MUL);
}

TEST_CASE("Decoder decodes MULH instructions") {
    require_r_type_decode<MULH>(0x33U, 0b001U, 0b0000001U, Register::s2, Register::s3, Register::s4, Opcode::MULH);
}

TEST_CASE("Decoder decodes MULHSU instructions") {
    require_r_type_decode<MULHSU>(0x33U, 0b010U, 0b0000001U, Register::s5, Register::s6, Register::s7, Opcode::MULHSU);
}

TEST_CASE("Decoder decodes MULHU instructions") {
    require_r_type_decode<MULHU>(0x33U, 0b011U, 0b0000001U, Register::s8, Register::s9, Register::s10, Opcode::MULHU);
}

TEST_CASE("Decoder decodes DIV instructions") {
    require_r_type_decode<DIV>(0x33U, 0b100U, 0b0000001U, Register::s11, Register::t0, Register::t1, Opcode::DIV);
}

TEST_CASE("Decoder decodes DIVU instructions") {
    require_r_type_decode<DIVU>(0x33U, 0b101U, 0b0000001U, Register::t2, Register::t3, Register::t4, Opcode::DIVU);
}

TEST_CASE("Decoder decodes REM instructions") {
    require_r_type_decode<REM>(0x33U, 0b110U, 0b0000001U, Register::t5, Register::t6, Register::ra, Opcode::REM);
}

TEST_CASE("Decoder decodes REMU instructions") {
    require_r_type_decode<REMU>(0x33U, 0b111U, 0b0000001U, Register::sp, Register::gp, Register::tp, Opcode::REMU);
}

TEST_CASE("Decoder decodes MULW instructions") {
    require_r_type_decode<MULW>(0x3bU, 0b000U, 0b0000001U, Register::t0, Register::t1, Register::t2, Opcode::MULW);
}

TEST_CASE("Decoder decodes DIVW instructions") {
    require_r_type_decode<DIVW>(0x3bU, 0b100U, 0b0000001U, Register::t3, Register::t4, Register::t5, Opcode::DIVW);
}

TEST_CASE("Decoder decodes DIVUW instructions") {
    require_r_type_decode<DIVUW>(0x3bU, 0b101U, 0b0000001U, Register::t6, Register::a0, Register::a1, Opcode::DIVUW);
}

TEST_CASE("Decoder decodes REMW instructions") {
    require_r_type_decode<REMW>(0x3bU, 0b110U, 0b0000001U, Register::a2, Register::a3, Register::a4, Opcode::REMW);
}

TEST_CASE("Decoder decodes REMUW instructions") {
    require_r_type_decode<REMUW>(0x3bU, 0b111U, 0b0000001U, Register::a5, Register::a6, Register::a7, Opcode::REMUW);
}

TEST_CASE("Decoder returns nullptr for invalid OP funct7 and funct3 combinations") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_r_type(0x33U, 0b010U, 0b0100000U, Register::a0, Register::a1, Register::a2));

    REQUIRE(instruction == nullptr);
}

TEST_CASE("Decoder returns nullptr for invalid OP-32 funct7 and funct3 combinations") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_r_type(0x3bU, 0b001U, 0b0100000U, Register::a3, Register::a4, Register::a5));

    REQUIRE(instruction == nullptr);
}

TEST_CASE("Decoder does not decode compressed words as R-type instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(0x00002000U);

    REQUIRE(instruction == nullptr);
}

TEST_CASE("Decoder only decodes the first compressed instruction from a 32-bit word before standard R-type dispatch") {
    Decoder decoder;
    const uint32_t first = 0x0001U;
    const uint32_t second = encode_r_type(0x33U, 0b000U, 0b0000000U, Register::a0, Register::a1, Register::a2);
    const auto instruction = decoder.decode(first | (second << 16U));

    REQUIRE(instruction != nullptr);
    const auto* compressed = dynamic_cast<const C_NOP*>(instruction.get());
    REQUIRE(compressed != nullptr);
}
