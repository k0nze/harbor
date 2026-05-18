#include "harbor/mmio/register_file.h"

#include <cassert>
#include <cstdint>
#include <optional>

namespace {

void reset_initializes_all_registers_to_zero()
{
    harbor::mmio::RegisterFile registers;

    for (std::uint64_t offset = 0; offset < harbor::mmio::RegisterFile::AddressSpanBytes;
         offset += harbor::mmio::RegisterFile::RegisterWidthBytes) {
        assert(registers.read32(offset) == std::optional<std::uint32_t>{0U});
    }
}

void writes_are_visible_to_reads()
{
    harbor::mmio::RegisterFile registers;

    assert(registers.write32(0x00, 0x12345678U));
    assert(registers.write32(0x3c, 0xa5a55a5aU));

    assert(registers.read32(0x00) == std::optional<std::uint32_t>{0x12345678U});
    assert(registers.read32(0x3c) == std::optional<std::uint32_t>{0xa5a55a5aU});
}

void reset_clears_written_values()
{
    harbor::mmio::RegisterFile registers;

    assert(registers.write32(0x10, 0xffffffffU));
    registers.reset();

    assert(registers.read32(0x10) == std::optional<std::uint32_t>{0U});
}

void rejects_unaligned_accesses()
{
    harbor::mmio::RegisterFile registers;

    assert(!registers.write32(0x01, 0x12345678U));
    assert(!registers.read32(0x02).has_value());
}

void rejects_out_of_range_accesses()
{
    harbor::mmio::RegisterFile registers;

    assert(!registers.write32(harbor::mmio::RegisterFile::AddressSpanBytes, 0x12345678U));
    assert(!registers.read32(harbor::mmio::RegisterFile::AddressSpanBytes).has_value());
}

} // namespace

int main()
{
    reset_initializes_all_registers_to_zero();
    writes_are_visible_to_reads();
    reset_clears_written_values();
    rejects_unaligned_accesses();
    rejects_out_of_range_accesses();
    return 0;
}
