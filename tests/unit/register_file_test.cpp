#include "harbor/mmio/register_file.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <optional>

namespace {

std::uint64_t register_offset(std::size_t index) {
    return static_cast<std::uint64_t>(index * harbor::mmio::RegisterFile::RegisterWidthBytes);
}

std::uint32_t test_value_for_register(std::size_t index) {
    return UINT32_C(0xa5000000) | static_cast<std::uint32_t>(index);
}

void reset_initializes_all_registers_to_expected_values() {
    harbor::mmio::RegisterFile registers;

    for (std::uint64_t offset = 0; offset < harbor::mmio::RegisterFile::AddressSpanBytes;
         offset += harbor::mmio::RegisterFile::RegisterWidthBytes) {
        const auto index = static_cast<std::size_t>(offset / harbor::mmio::RegisterFile::RegisterWidthBytes);
        assert(registers.read32(offset) == std::optional<std::uint32_t>{
                                             harbor::mmio::RegisterFile::reset_value_for_register_index(index)});
    }
}

void writes_are_visible_to_reads() {
    harbor::mmio::RegisterFile registers;
    std::size_t written_register_count = 0;

    for (std::size_t index = 0; index < harbor::mmio::RegisterFile::RegisterCount; ++index) {
        if (harbor::mmio::RegisterFile::is_constant_register_index(index)) {
            continue;
        }

        const auto offset = register_offset(index);
        const auto value = test_value_for_register(index);

        assert(registers.write32(offset, value));
        assert(registers.read32(offset) == std::optional<std::uint32_t>{value});
        ++written_register_count;
    }

    assert(written_register_count > 0U);
}

void reset_clears_written_values() {
    harbor::mmio::RegisterFile registers;
    bool tested_writable_register = false;

    for (std::size_t index = 0; index < harbor::mmio::RegisterFile::RegisterCount; ++index) {
        if (harbor::mmio::RegisterFile::is_constant_register_index(index)) {
            continue;
        }

        const auto offset = register_offset(index);

        assert(registers.write32(offset, 0xffffffffU));
        registers.reset();
        assert(registers.read32(offset) == std::optional<std::uint32_t>{
                                             harbor::mmio::RegisterFile::reset_value_for_register_index(index)});
        tested_writable_register = true;
        break;
    }

    assert(tested_writable_register);
}

void accepts_but_ignores_writes_to_constant_registers() {
    harbor::mmio::RegisterFile registers;
    std::size_t constant_register_count = 0;

    for (std::size_t index = 0; index < harbor::mmio::RegisterFile::RegisterCount; ++index) {
        if (!harbor::mmio::RegisterFile::is_constant_register_index(index)) {
            continue;
        }

        const auto offset = register_offset(index);
        const auto value = harbor::mmio::RegisterFile::reset_value_for_register_index(index);

        assert(registers.read32(offset) == std::optional<std::uint32_t>{value});
        assert(registers.write32(offset, 0xffffffffU));
        assert(registers.read32(offset) == std::optional<std::uint32_t>{value});
        ++constant_register_count;
    }

    assert(constant_register_count == HARBOR_MMIO_REGISTER_FILE_CONSTANT_REGISTER_COUNT);
}

void rejects_unaligned_accesses() {
    harbor::mmio::RegisterFile registers;

    assert(!registers.write32(0x01, 0x12345678U));
    assert(!registers.read32(0x02).has_value());
}

void rejects_out_of_range_accesses() {
    harbor::mmio::RegisterFile registers;

    assert(!registers.write32(harbor::mmio::RegisterFile::AddressSpanBytes, 0x12345678U));
    assert(!registers.read32(harbor::mmio::RegisterFile::AddressSpanBytes).has_value());
}

void exposes_expected_default_memory_map() {
    static_assert(harbor::mmio::RegisterFile::DefaultBaseAddress == 0x10010000ULL);
    static_assert(harbor::mmio::RegisterFile::RegisterCount == 16U);
    static_assert(harbor::mmio::RegisterFile::RegisterWidthBytes == 4U);
    static_assert(harbor::mmio::RegisterFile::AddressSpanBytes == 0x40U);
    static_assert(harbor::mmio::RegisterFile::ConstantRegisterMask ==
                  HARBOR_MMIO_REGISTER_FILE_CONSTANT_REGISTER_MASK);

    std::size_t constant_register_count = 0;

    for (std::size_t index = 0; index < harbor::mmio::RegisterFile::RegisterCount; ++index) {
        const auto map_index = static_cast<std::uint32_t>(index);
        const bool expected_constant =
            harbor_mmio_register_file_is_constant_register_index(map_index) != 0;

        assert(harbor::mmio::RegisterFile::is_constant_register_index(index) ==
               expected_constant);
        assert(harbor::mmio::RegisterFile::reset_value_for_register_index(index) ==
               harbor_mmio_register_file_constant_register_value(map_index));

        if (expected_constant) {
            ++constant_register_count;
        }
    }

    assert(constant_register_count == HARBOR_MMIO_REGISTER_FILE_CONSTANT_REGISTER_COUNT);
}

} // namespace

int main() {
    reset_initializes_all_registers_to_expected_values();
    writes_are_visible_to_reads();
    reset_clears_written_values();
    accepts_but_ignores_writes_to_constant_registers();
    rejects_unaligned_accesses();
    rejects_out_of_range_accesses();
    exposes_expected_default_memory_map();
    return 0;
}
