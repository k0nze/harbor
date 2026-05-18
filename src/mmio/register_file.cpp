#include "harbor/mmio/register_file.h"

#include <algorithm>

namespace harbor::mmio {

RegisterFile::RegisterFile()
{
    reset();
}

void RegisterFile::reset()
{
    std::fill(registers_.begin(), registers_.end(), 0U);
}

std::optional<std::uint32_t> RegisterFile::read32(std::uint64_t offset) const
{
    if (!is_valid_register_offset(offset)) {
        return std::nullopt;
    }

    return registers_.at(static_cast<std::size_t>(offset / RegisterWidthBytes));
}

bool RegisterFile::write32(std::uint64_t offset, std::uint32_t value)
{
    if (!is_valid_register_offset(offset)) {
        return false;
    }

    registers_.at(static_cast<std::size_t>(offset / RegisterWidthBytes)) = value;
    return true;
}

bool RegisterFile::is_valid_register_offset(std::uint64_t offset)
{
    return offset < AddressSpanBytes && (offset % RegisterWidthBytes) == 0;
}

} // namespace harbor::mmio
