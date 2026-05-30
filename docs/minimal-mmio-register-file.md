# Minimal MMIO Register File

The first Harbor MMIO test device is a 16-entry register file.

## Register Layout

- Register width: 32 bits.
- Register count: 16.
- MMIO window size: 64 bytes.
- Valid offsets: `0x00`, `0x04`, `0x08`, ..., `0x3c`.
- Reset value: `0x00000000` for every register.
- Access rule: only aligned 32-bit accesses are valid.

## Proposed Test Mapping

The current guest-side `mmio-test` uses `0x10010000` as the test address. The
Harbor-enabled QEMU `virt` build maps the register file as:

```text
base: 0x10010000
size: 0x40
```

The first real guest-side validation should then be:

```sh
mmio-test write 0x10010000 0x12345678
mmio-test read  0x10010000
```

The expected readback value is `0x12345678`.

## Current Implementation Boundary

The Harbor-side model is implemented by `harbor::mmio::RegisterFile`. QEMU does
not implement register behavior directly. Instead, the QEMU C shim calls the
Harbor QEMU adapter C ABI declared in `include/harbor/qemu/mmio_adapter.h`,
and the adapter forwards the access into the Harbor C++ model.

The shared memory-map constants are declared in
`include/harbor/mmio/register_file_map.h` and are used by the C++ model, QEMU
shim, and Buildroot guest `mmio-test` utility.
