# Harbor

## Setup

### macOS

```bash
brew install cmake gmp z3 pkgconf dune opam ocaml riscv64-elf-gcc dtc curl gnu-tar make
```

Put Homebrew's GNU tools before the default macOS tools when using the upstream
Sail RISC-V Linux boot Makefile. The upstream Makefile requires GNU Make 4.0 or
newer and uses `tar --touch`, which BSD tar does not support:

```bash
export PATH="$(brew --prefix make)/libexec/gnubin:$PATH"
export PATH="$(brew --prefix gnu-tar)/libexec/gnubin:$PATH"
```

### General

Initialize opam and create a project switch:

```bash
opam init
opam switch create harbor 4.14.2
eval $(opam env --switch=harbor)
```

Install Sail’s OCaml dependencies from the Sail submodule:

```bash
pushd external/sail
opam install . --deps-only
opam install menhir.20240715
popd
```

Reconfigure Harbor with the opam environment active:

```bash
eval $(opam env --switch=harbor)
cmake -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

Optionally set a custom Sail installation prefix:

```bash
cmake -B build -DHARBOR_SAIL_INSTALL_PREFIX=/path/to/sail-prefix
```

If this option is omitted, Harbor installs Sail into `toolchain/sail` inside the
repository.

Build the Sail compiler:

```bash
cmake --build build --target harbor_sail_compiler
```

Install the Sail compiler into Harbor's local toolchain directory:

```bash
cmake --build build --target harbor_sail_install
```

Check that the Sail installation was successful:

```bash
toolchain/sail/bin/sail --version
```

Build the Sail RISC-V model library:

```bash
cmake --build build --target harbor_riscv_model
```

The first build may download Sail RISC-V CMake dependencies into the build tree.

## Use Sail RISC-V Emulator

Build the standalone Sail RISC-V simulator executable:

```bash
cmake --build build/sail-riscv --target sail_riscv_sim
```

The executable is generated at:

```bash
build/sail-riscv/c_emulator/sail_riscv_sim
```

Check that it runs:

```bash
build/sail-riscv/c_emulator/sail_riscv_sim --version
build/sail-riscv/c_emulator/sail_riscv_sim --print-isa-string
```

List all supported command-line options:

```bash
build/sail-riscv/c_emulator/sail_riscv_sim --help
```

Run a RISC-V ELF file:

```bash
build/sail-riscv/c_emulator/sail_riscv_sim path/to/program.elf
```

Multiple ELF files can be passed. They are loaded in order, and the program
counter is set to the entry point of the first ELF.

Useful options while bringing up bare-metal examples:

```bash
build/sail-riscv/c_emulator/sail_riscv_sim --inst-limit 1000 path/to/program.elf
build/sail-riscv/c_emulator/sail_riscv_sim --trace-instr --inst-limit 100 path/to/program.elf
build/sail-riscv/c_emulator/sail_riscv_sim --trace path/to/program.elf
```

The Harbor-owned bare-metal examples will live under `examples/baremetal`.
The current minimal example can be built and run with:

```bash
cmake --build build --target harbor_baremetal_minimal
cmake --build build --target harbor_run_baremetal_minimal
```

The current C hello example can be built and run with:

```bash
cmake --build build --target harbor_baremetal_hello
cmake --build build --target harbor_run_baremetal_hello
```

## Run Linux on Sail RISC-V Emulator

Build the standalone Sail RISC-V simulator if it is not already built:

```bash
cmake --build build/sail-riscv --target sail_riscv_sim
```

Build the upstream OpenSBI + Linux payload:

```bash
make -C external/sail-riscv/os-boot/linux -j4
```

This downloads the upstream boot dependencies into
`external/sail-riscv/os-boot/linux/downloads` and creates the bootable ELF:

```bash
external/sail-riscv/os-boot/linux/build/fw_payload.elf
```

Run the image with Harbor's Sail RISC-V simulator build:

```bash
make -C external/sail-riscv/os-boot/linux sail \
  SAIL_SIM="$PWD/build/sail-riscv/c_emulator/sail_riscv_sim"
```

For a shorter first run, override the instruction limit:

```bash
make -C external/sail-riscv/os-boot/linux sail \
  SAIL_SIM="$PWD/build/sail-riscv/c_emulator/sail_riscv_sim" \
  INSTRUCTION_LIMIT=1000000
```

The run target generates a device tree from the emulator, compiles it to
`external/sail-riscv/os-boot/linux/build/sail.dtb`, and passes it to the
emulator with `--device-tree-blob`.

Expected behavior: OpenSBI should print its banner first, then Linux should
start booting. The upstream image currently does not include a userspace `init`,
so a full run eventually fails when Linux cannot start init.

## Helpful links:

 * [rv32i/rv64i instructions](https://msyksphinz-self.github.io/riscv-isadoc/html/rvi.html)
 * [encoder/decoder](https://luplab.gitlab.io/rvcodecjs/#q=ecall&abi=false&isa=AUTO)
