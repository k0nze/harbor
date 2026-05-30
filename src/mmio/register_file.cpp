#include "harbor/mmio/register_file.h"

#include <limits>

namespace harbor::mmio {

RegisterFile::RegisterFile() {
    reset();
}

void RegisterFile::reset() {
    for (std::size_t index = 0; index < registers_.size(); ++index) {
        registers_.at(index) = reset_value_for_register_index(index);
    }
}

std::optional<std::uint32_t> RegisterFile::read32(std::uint64_t offset) const {
    if (!is_valid_register_offset(offset)) {
        return std::nullopt;
    }

    return registers_.at(static_cast<std::size_t>(offset / RegisterWidthBytes));
}

bool RegisterFile::write32(std::uint64_t offset, std::uint32_t value) {
    if (!is_valid_register_offset(offset)) {
        return false;
    }

    const auto index = static_cast<std::size_t>(offset / RegisterWidthBytes);
    if (is_constant_register_index(index)) {
        return true;
    }

    registers_.at(index) = value;
    return true;
}

bool RegisterFile::is_valid_register_offset(std::uint64_t offset) {
    return offset < AddressSpanBytes && (offset % RegisterWidthBytes) == 0;
}

bool RegisterFile::is_constant_register_index(std::size_t index) {
    if (index > std::numeric_limits<std::uint32_t>::max()) {
        return false;
    }

    return harbor_mmio_register_file_is_constant_register_index(
               static_cast<std::uint32_t>(index)) != 0;
}

std::uint32_t RegisterFile::reset_value_for_register_index(std::size_t index) {
    if (index > std::numeric_limits<std::uint32_t>::max()) {
        return 0U;
    }

    return harbor_mmio_register_file_constant_register_value(static_cast<std::uint32_t>(index));
}

} // namespace harbor::mmio
