# Harbor

## Setup

### macOS

```bash
brew install cmake gmp z3 pkgconf dune opam ocaml
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

## Helpful links:

 * [rv32i/rv64i instructions](https://msyksphinz-self.github.io/riscv-isadoc/html/rvi.html)
 * [encoder/decoder](https://luplab.gitlab.io/rvcodecjs/#q=ecall&abi=false&isa=AUTO)
