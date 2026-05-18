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

The current guest-side `mmio-test` uses `0x10010000` as the placeholder test
address. Once a QEMU adapter exists, map the register file as:

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

The Harbor-side model is implemented by `harbor::mmio::RegisterFile`.

This model does not yet make the registers visible to the guest. A QEMU device
or bridge still needs to decode the guest physical address range and forward
32-bit reads and writes to the model.
