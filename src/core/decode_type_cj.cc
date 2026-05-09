#include "decoder_internal.h"

#include "rviss/isa/c_j.h"

std::unique_ptr<Instruction> decode_type_cj(uint16_t encoded_instruction) {
    const uint16_t imm = static_cast<uint16_t>((((encoded_instruction >> 12U) & 0x1U) << 11U)
        | (((encoded_instruction >> 11U) & 0x1U) << 4U)
        | (((encoded_instruction >> 9U) & 0x3U) << 8U)
        | (((encoded_instruction >> 8U) & 0x1U) << 10U)
        | (((encoded_instruction >> 7U) & 0x1U) << 6U)
        | (((encoded_instruction >> 6U) & 0x1U) << 7U)
        | (((encoded_instruction >> 3U) & 0x7U) << 1U)
        | (((encoded_instruction >> 2U) & 0x1U) << 5U));

    return std::make_unique<C_J>(imm);
}
