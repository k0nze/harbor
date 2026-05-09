#include "decoder_internal.h"

#include "rviss/isa/c_add.h"
#include "rviss/isa/c_ebreak.h"
#include "rviss/isa/c_jalr.h"
#include "rviss/isa/c_jr.h"
#include "rviss/isa/c_mv.h"
#include "rviss/isa/register.h"

namespace {

Register decode_register(uint16_t bits) {
    return static_cast<Register>(bits & 0x1fU);
}

}

std::unique_ptr<Instruction> decode_type_cr(uint16_t encoded_instruction) {
    const Register rd_rs1 = decode_register(static_cast<uint16_t>(encoded_instruction >> 7U));
    const Register rs2 = decode_register(static_cast<uint16_t>(encoded_instruction >> 2U));
    const uint16_t bit12 = static_cast<uint16_t>((encoded_instruction >> 12U) & 0x1U);

    if (bit12 == 0U) {
        if (rs2 == Register::zero) {
            if (rd_rs1 == Register::zero) {
                return nullptr;
            }

            return std::make_unique<C_JR>(rd_rs1);
        }

        return std::make_unique<C_MV>(rd_rs1, rs2);
    }

    if (rs2 == Register::zero) {
        if (rd_rs1 == Register::zero) {
            return std::make_unique<C_EBREAK>();
        }

        return std::make_unique<C_JALR>(rd_rs1);
    }

    return std::make_unique<C_ADD>(rd_rs1, rs2);
}
