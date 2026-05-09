#include "decoder_internal.h"

#include "rviss/isa/c_addi4spn.h"
#include "rviss/isa/register.h"

namespace {

Register decode_compact_register(uint16_t bits) {
    return static_cast<Register>(8U + (bits & 0x7U));
}

uint16_t decode_ciw_immediate(uint16_t encoded_instruction) {
    return static_cast<uint16_t>((((encoded_instruction >> 11U) & 0x3U) << 4U)
        | (((encoded_instruction >> 7U) & 0xfU) << 6U)
        | (((encoded_instruction >> 6U) & 0x1U) << 2U)
        | (((encoded_instruction >> 5U) & 0x1U) << 3U));
}

}

std::unique_ptr<Instruction> decode_type_ciw(uint16_t encoded_instruction) {
    const Register rd = decode_compact_register(static_cast<uint16_t>(encoded_instruction >> 2U));

    return std::make_unique<C_ADDI4SPN>(rd, decode_ciw_immediate(encoded_instruction));
}
