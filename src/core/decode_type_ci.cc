#include "decoder_internal.h"

#include "rviss/isa/c_addi.h"
#include "rviss/isa/c_addi16sp.h"
#include "rviss/isa/c_addiw.h"
#include "rviss/isa/c_ldsp.h"
#include "rviss/isa/c_li.h"
#include "rviss/isa/c_lui.h"
#include "rviss/isa/c_lwsp.h"
#include "rviss/isa/c_nop.h"
#include "rviss/isa/c_slli.h"
#include "rviss/isa/register.h"

namespace {

Register decode_register(uint16_t bits) {
    return static_cast<Register>(bits & 0x1fU);
}

uint16_t decode_ci_immediate(uint16_t encoded_instruction) {
    return static_cast<uint16_t>((((encoded_instruction >> 12U) & 0x1U) << 5U)
        | ((encoded_instruction >> 2U) & 0x1fU));
}

}

std::unique_ptr<Instruction> decode_type_ci(uint16_t encoded_instruction) {
    const uint16_t quadrant = static_cast<uint16_t>(encoded_instruction & 0x3U);
    const uint16_t funct3 = static_cast<uint16_t>((encoded_instruction >> 13U) & 0x7U);
    const Register rd_rs1 = decode_register(static_cast<uint16_t>(encoded_instruction >> 7U));
    const uint16_t imm = decode_ci_immediate(encoded_instruction);

    if (quadrant == 0b01U) {
        switch (funct3) {
        case 0b000U:
            if (rd_rs1 == Register::zero && imm == 0U) {
                return std::make_unique<C_NOP>();
            }

            return std::make_unique<C_ADDI>(rd_rs1, imm);
        case 0b001U:
            return std::make_unique<C_ADDIW>(rd_rs1, imm);
        case 0b010U:
            return std::make_unique<C_LI>(rd_rs1, imm);
        case 0b011U:
            if (rd_rs1 == Register::sp) {
                return std::make_unique<C_ADDI16SP>(imm);
            }

            return std::make_unique<C_LUI>(rd_rs1, imm);
        default:
            return nullptr;
        }
    }

    if (quadrant == 0b10U) {
        switch (funct3) {
        case 0b000U:
            return std::make_unique<C_SLLI>(rd_rs1, imm);
        case 0b010U:
            return std::make_unique<C_LWSP>(rd_rs1, imm);
        case 0b011U:
            return std::make_unique<C_LDSP>(rd_rs1, imm);
        default:
            return nullptr;
        }
    }

    return nullptr;
}
