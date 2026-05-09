#include "decoder_internal.h"

#include "rviss/isa/add.h"
#include "rviss/isa/addw.h"
#include "rviss/isa/and.h"
#include "rviss/isa/div.h"
#include "rviss/isa/divu.h"
#include "rviss/isa/divuw.h"
#include "rviss/isa/divw.h"
#include "rviss/isa/mul.h"
#include "rviss/isa/mulh.h"
#include "rviss/isa/mulhsu.h"
#include "rviss/isa/mulhu.h"
#include "rviss/isa/mulw.h"
#include "rviss/isa/or.h"
#include "rviss/isa/register.h"
#include "rviss/isa/rem.h"
#include "rviss/isa/remu.h"
#include "rviss/isa/remuw.h"
#include "rviss/isa/remw.h"
#include "rviss/isa/sll.h"
#include "rviss/isa/sllw.h"
#include "rviss/isa/slt.h"
#include "rviss/isa/sltu.h"
#include "rviss/isa/sra.h"
#include "rviss/isa/sraw.h"
#include "rviss/isa/srl.h"
#include "rviss/isa/srlw.h"
#include "rviss/isa/sub.h"
#include "rviss/isa/subw.h"
#include "rviss/isa/xor.h"

namespace {

Register decode_register(uint32_t bits) {
    return static_cast<Register>(bits & 0x1fU);
}

std::unique_ptr<Instruction> decode_op(Register rd, Register rs1, Register rs2, uint32_t funct3, uint32_t funct7) {
    switch (funct7) {
    case 0b0000000U:
        switch (funct3) {
        case 0b000U:
            return std::make_unique<ADD>(rd, rs1, rs2);
        case 0b001U:
            return std::make_unique<SLL>(rd, rs1, rs2);
        case 0b010U:
            return std::make_unique<SLT>(rd, rs1, rs2);
        case 0b011U:
            return std::make_unique<SLTU>(rd, rs1, rs2);
        case 0b100U:
            return std::make_unique<XOR>(rd, rs1, rs2);
        case 0b101U:
            return std::make_unique<SRL>(rd, rs1, rs2);
        case 0b110U:
            return std::make_unique<OR>(rd, rs1, rs2);
        case 0b111U:
            return std::make_unique<AND>(rd, rs1, rs2);
        default:
            return nullptr;
        }
    case 0b0100000U:
        switch (funct3) {
        case 0b000U:
            return std::make_unique<SUB>(rd, rs1, rs2);
        case 0b101U:
            return std::make_unique<SRA>(rd, rs1, rs2);
        default:
            return nullptr;
        }
    case 0b0000001U:
        switch (funct3) {
        case 0b000U:
            return std::make_unique<MUL>(rd, rs1, rs2);
        case 0b001U:
            return std::make_unique<MULH>(rd, rs1, rs2);
        case 0b010U:
            return std::make_unique<MULHSU>(rd, rs1, rs2);
        case 0b011U:
            return std::make_unique<MULHU>(rd, rs1, rs2);
        case 0b100U:
            return std::make_unique<DIV>(rd, rs1, rs2);
        case 0b101U:
            return std::make_unique<DIVU>(rd, rs1, rs2);
        case 0b110U:
            return std::make_unique<REM>(rd, rs1, rs2);
        case 0b111U:
            return std::make_unique<REMU>(rd, rs1, rs2);
        default:
            return nullptr;
        }
    default:
        return nullptr;
    }
}

std::unique_ptr<Instruction> decode_op_32(Register rd, Register rs1, Register rs2, uint32_t funct3, uint32_t funct7) {
    switch (funct7) {
    case 0b0000000U:
        switch (funct3) {
        case 0b000U:
            return std::make_unique<ADDW>(rd, rs1, rs2);
        case 0b001U:
            return std::make_unique<SLLW>(rd, rs1, rs2);
        case 0b101U:
            return std::make_unique<SRLW>(rd, rs1, rs2);
        default:
            return nullptr;
        }
    case 0b0100000U:
        switch (funct3) {
        case 0b000U:
            return std::make_unique<SUBW>(rd, rs1, rs2);
        case 0b101U:
            return std::make_unique<SRAW>(rd, rs1, rs2);
        default:
            return nullptr;
        }
    case 0b0000001U:
        switch (funct3) {
        case 0b000U:
            return std::make_unique<MULW>(rd, rs1, rs2);
        case 0b100U:
            return std::make_unique<DIVW>(rd, rs1, rs2);
        case 0b101U:
            return std::make_unique<DIVUW>(rd, rs1, rs2);
        case 0b110U:
            return std::make_unique<REMW>(rd, rs1, rs2);
        case 0b111U:
            return std::make_unique<REMUW>(rd, rs1, rs2);
        default:
            return nullptr;
        }
    default:
        return nullptr;
    }
}

}

std::unique_ptr<Instruction> decode_type_r(uint32_t encoded_instruction) {
    const Register rd = decode_register(encoded_instruction >> 7U);
    const uint32_t funct3 = (encoded_instruction >> 12U) & 0x7U;
    const Register rs1 = decode_register(encoded_instruction >> 15U);
    const Register rs2 = decode_register(encoded_instruction >> 20U);
    const uint32_t funct7 = (encoded_instruction >> 25U) & 0x7fU;

    switch (encoded_instruction & 0x7fU) {
    case 0x33U:
        return decode_op(rd, rs1, rs2, funct3, funct7);
    case 0x3bU:
        return decode_op_32(rd, rs1, rs2, funct3, funct7);
    default:
        return nullptr;
    }
}
