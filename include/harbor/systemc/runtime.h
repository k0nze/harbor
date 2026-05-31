#ifndef HARBOR_SYSTEMC_RUNTIME_H
#define HARBOR_SYSTEMC_RUNTIME_H

namespace harbor::systemc {

/**
 * @brief Return the SystemC runtime version linked into Harbor.
 *
 * This function is intentionally small: it gives the Harbor SystemC library a
 * concrete, testable dependency on the host SystemC installation without
 * coupling QEMU adapter code to SystemC headers.
 *
 * @return Null-terminated SystemC version string owned by the SystemC library.
 */
[[nodiscard]] const char *runtime_version();

} // namespace harbor::systemc

#endif // HARBOR_SYSTEMC_RUNTIME_H
