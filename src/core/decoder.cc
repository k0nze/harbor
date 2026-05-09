#include "rviss/core/decoder.h"

#include "decoder_internal.h"

std::unique_ptr<Instruction> Decoder::decode(uint32_t encoded_instruction) const {
    if ((encoded_instruction & 0b11U) != 0b11U) {
        const uint16_t compressed_instruction = static_cast<uint16_t>(encoded_instruction & 0xffffU);
        const uint16_t quadrant = static_cast<uint16_t>(compressed_instruction & 0x3U);
        const uint16_t funct3 = static_cast<uint16_t>((compressed_instruction >> 13U) & 0x7U);

        if (quadrant == 0b01U) {
            if (funct3 == 0b000U || funct3 == 0b001U || funct3 == 0b010U || funct3 == 0b011U) {
                return decode_type_ci(compressed_instruction);
            }

            if (funct3 == 0b100U) {
                if (((compressed_instruction >> 10U) & 0x3U) == 0b11U) {
                    return decode_type_ca(compressed_instruction);
                }

                return decode_type_cb(compressed_instruction);
            }

            if (funct3 == 0b110U || funct3 == 0b111U) {
                return decode_type_cb(compressed_instruction);
            }

            if (funct3 == 0b101U) {
                return decode_type_cj(compressed_instruction);
            }
        }

        if (quadrant == 0b10U) {
            if (funct3 == 0b000U || funct3 == 0b010U || funct3 == 0b011U) {
                return decode_type_ci(compressed_instruction);
            }

            if (funct3 == 0b100U) {
                return decode_type_cr(compressed_instruction);
            }
        }

        return nullptr;
    }

    switch (encoded_instruction & 0x7fU) {
    case 0x63U:
        return decode_type_b(encoded_instruction);
    case 0x2fU:
        return decode_type_amo(encoded_instruction);
    default:
        return nullptr;
    }
}
