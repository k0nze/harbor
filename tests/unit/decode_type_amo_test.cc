#include <catch2/catch_test_macros.hpp>

#include "rviss/core/decoder.h"
#include "rviss/rviss.h"

namespace {

uint32_t encode_amo_type(uint32_t funct5, uint32_t funct3, Register rd, Register rs1, Register rs2, uint32_t aqrl = 0U) {
    return ((funct5 & 0x1fU) << 27U)
        | ((aqrl & 0x3U) << 25U)
        | ((static_cast<uint32_t>(rs2) & 0x1fU) << 20U)
        | ((static_cast<uint32_t>(rs1) & 0x1fU) << 15U)
        | ((funct3 & 0x7U) << 12U)
        | ((static_cast<uint32_t>(rd) & 0x1fU) << 7U)
        | 0x2fU;
}

}

TEST_CASE("Decoder decodes LR_W instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b00010U, 0b010U, Register::a0, Register::a1, Register::zero));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const LR_W*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::a0);
    REQUIRE(amo->rs1 == Register::a1);
    REQUIRE(amo->rs2 == Register::zero);
}

TEST_CASE("Decoder decodes SC_W instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b00011U, 0b010U, Register::a2, Register::a3, Register::a4));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const SC_W*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::a2);
    REQUIRE(amo->rs1 == Register::a3);
    REQUIRE(amo->rs2 == Register::a4);
}

TEST_CASE("Decoder decodes AMOSWAP_W instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b00001U, 0b010U, Register::a5, Register::a6, Register::a7));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const AMOSWAP_W*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::a5);
    REQUIRE(amo->rs1 == Register::a6);
    REQUIRE(amo->rs2 == Register::a7);
}

TEST_CASE("Decoder decodes AMOADD_W instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b00000U, 0b010U, Register::s0, Register::s1, Register::s2));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const AMOADD_W*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::s0);
    REQUIRE(amo->rs1 == Register::s1);
    REQUIRE(amo->rs2 == Register::s2);
}

TEST_CASE("Decoder decodes AMOXOR_W instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b00100U, 0b010U, Register::s3, Register::s4, Register::s5));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const AMOXOR_W*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::s3);
    REQUIRE(amo->rs1 == Register::s4);
    REQUIRE(amo->rs2 == Register::s5);
}

TEST_CASE("Decoder decodes AMOAND_W instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b01100U, 0b010U, Register::s6, Register::s7, Register::s8));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const AMOAND_W*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::s6);
    REQUIRE(amo->rs1 == Register::s7);
    REQUIRE(amo->rs2 == Register::s8);
}

TEST_CASE("Decoder decodes AMOOR_W instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b01000U, 0b010U, Register::s9, Register::s10, Register::s11));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const AMOOR_W*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::s9);
    REQUIRE(amo->rs1 == Register::s10);
    REQUIRE(amo->rs2 == Register::s11);
}

TEST_CASE("Decoder decodes AMOMIN_W instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b10000U, 0b010U, Register::t0, Register::t1, Register::t2));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const AMOMIN_W*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::t0);
    REQUIRE(amo->rs1 == Register::t1);
    REQUIRE(amo->rs2 == Register::t2);
}

TEST_CASE("Decoder decodes AMOMAX_W instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b10100U, 0b010U, Register::t3, Register::t4, Register::t5));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const AMOMAX_W*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::t3);
    REQUIRE(amo->rs1 == Register::t4);
    REQUIRE(amo->rs2 == Register::t5);
}

TEST_CASE("Decoder decodes AMOMINU_W instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b11000U, 0b010U, Register::t6, Register::a0, Register::a1));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const AMOMINU_W*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::t6);
    REQUIRE(amo->rs1 == Register::a0);
    REQUIRE(amo->rs2 == Register::a1);
}

TEST_CASE("Decoder decodes AMOMAXU_W instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b11100U, 0b010U, Register::a2, Register::a3, Register::a4));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const AMOMAXU_W*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::a2);
    REQUIRE(amo->rs1 == Register::a3);
    REQUIRE(amo->rs2 == Register::a4);
}

TEST_CASE("Decoder decodes LR_D instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b00010U, 0b011U, Register::a5, Register::a6, Register::zero));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const LR_D*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::a5);
    REQUIRE(amo->rs1 == Register::a6);
    REQUIRE(amo->rs2 == Register::zero);
}

TEST_CASE("Decoder ignores encoded rs2 for LR_W and normalizes it to zero") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b00010U, 0b010U, Register::t0, Register::t1, Register::t2));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const LR_W*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::t0);
    REQUIRE(amo->rs1 == Register::t1);
    REQUIRE(amo->rs2 == Register::zero);
}

TEST_CASE("Decoder ignores aq and rl bits for LR_D") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b00010U, 0b011U, Register::t3, Register::t4, Register::zero, 0b11U));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const LR_D*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::t3);
    REQUIRE(amo->rs1 == Register::t4);
    REQUIRE(amo->rs2 == Register::zero);
}

TEST_CASE("Decoder decodes SC_D instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b00011U, 0b011U, Register::a7, Register::s0, Register::s1));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const SC_D*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::a7);
    REQUIRE(amo->rs1 == Register::s0);
    REQUIRE(amo->rs2 == Register::s1);
}

TEST_CASE("Decoder decodes AMOSWAP_D instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b00001U, 0b011U, Register::s2, Register::s3, Register::s4));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const AMOSWAP_D*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::s2);
    REQUIRE(amo->rs1 == Register::s3);
    REQUIRE(amo->rs2 == Register::s4);
}

TEST_CASE("Decoder decodes AMOADD_D instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b00000U, 0b011U, Register::s5, Register::s6, Register::s7));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const AMOADD_D*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::s5);
    REQUIRE(amo->rs1 == Register::s6);
    REQUIRE(amo->rs2 == Register::s7);
}

TEST_CASE("Decoder decodes AMOXOR_D instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b00100U, 0b011U, Register::s8, Register::s9, Register::s10));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const AMOXOR_D*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::s8);
    REQUIRE(amo->rs1 == Register::s9);
    REQUIRE(amo->rs2 == Register::s10);
}

TEST_CASE("Decoder decodes AMOAND_D instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b01100U, 0b011U, Register::s11, Register::t0, Register::t1));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const AMOAND_D*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::s11);
    REQUIRE(amo->rs1 == Register::t0);
    REQUIRE(amo->rs2 == Register::t1);
}

TEST_CASE("Decoder decodes AMOOR_D instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b01000U, 0b011U, Register::t2, Register::t3, Register::t4));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const AMOOR_D*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::t2);
    REQUIRE(amo->rs1 == Register::t3);
    REQUIRE(amo->rs2 == Register::t4);
}

TEST_CASE("Decoder decodes AMOMIN_D instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b10000U, 0b011U, Register::t5, Register::t6, Register::a0));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const AMOMIN_D*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::t5);
    REQUIRE(amo->rs1 == Register::t6);
    REQUIRE(amo->rs2 == Register::a0);
}

TEST_CASE("Decoder decodes AMOMAX_D instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b10100U, 0b011U, Register::a1, Register::a2, Register::a3));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const AMOMAX_D*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::a1);
    REQUIRE(amo->rs1 == Register::a2);
    REQUIRE(amo->rs2 == Register::a3);
}

TEST_CASE("Decoder decodes AMOMINU_D instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b11000U, 0b011U, Register::a4, Register::a5, Register::a6));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const AMOMINU_D*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::a4);
    REQUIRE(amo->rs1 == Register::a5);
    REQUIRE(amo->rs2 == Register::a6);
}

TEST_CASE("Decoder decodes AMOMAXU_D instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b11100U, 0b011U, Register::a7, Register::s0, Register::s1));

    REQUIRE(instruction != nullptr);
    const auto* amo = dynamic_cast<const AMOMAXU_D*>(instruction.get());
    REQUIRE(amo != nullptr);
    REQUIRE(amo->rd == Register::a7);
    REQUIRE(amo->rs1 == Register::s0);
    REQUIRE(amo->rs2 == Register::s1);
}

TEST_CASE("Decoder returns nullptr for invalid AMO funct5") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b00101U, 0b010U, Register::a0, Register::a1, Register::a2));

    REQUIRE(instruction == nullptr);
}

TEST_CASE("Decoder returns nullptr for invalid AMO width") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b00000U, 0b001U, Register::a0, Register::a1, Register::a2));

    REQUIRE(instruction == nullptr);
}

TEST_CASE("Decoder returns nullptr for another invalid AMO width") {
    Decoder decoder;
    const auto instruction = decoder.decode(encode_amo_type(0b00000U, 0b100U, Register::a3, Register::a4, Register::a5));

    REQUIRE(instruction == nullptr);
}

TEST_CASE("Decoder does not decode compressed words as AMO instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(0x00000000U);

    REQUIRE(instruction == nullptr);
}

TEST_CASE("Decoder does not decode non-AMO 32-bit opcodes as AMO instructions") {
    Decoder decoder;
    const auto instruction = decoder.decode(0x00000013U);

    REQUIRE(instruction == nullptr);
}
