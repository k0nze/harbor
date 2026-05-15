#!/usr/bin/env sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../../.." && pwd)

# Use the same pinned Buildroot version as fetch.sh unless the user overrides it.
version="${BUILDROOT_VERSION:-$(cat "${repo_root}/examples/linux/buildroot/buildroot.version")}"

# Buildroot supports out-of-tree builds; keep all generated files under build/.
source_dir="${BUILDROOT_SOURCE_DIR:-${repo_root}/.buildroot/buildroot-${version}}"
output_dir="${BUILDROOT_OUTPUT_DIR:-${repo_root}/build/buildroot/riscv64-qemu-virt}"
defconfig="${repo_root}/examples/linux/buildroot/riscv64_qemu_virt_defconfig"

# Tool setup is handled by the top-level README. These overrides are useful if
# a user wants a non-default GNU Make or host compiler.
make_cmd="${MAKE:-make}"
hostcc="${HOSTCC:-gcc}"
hostcxx="${HOSTCXX:-g++}"
bash_cmd="${BASH:-}"
# Buildroot's default AUTORECONF command hard-codes GTKDOCIZE=/bin/true.
# Use PATH-resolved true so the flow also works on hosts without /bin/true.
autoreconf_cmd='$(HOST_CONFIGURE_OPTS) ACLOCAL="$(ACLOCAL)" AUTOCONF="$(AUTOCONF)" AUTOHEADER="$(AUTOHEADER)" AUTOMAKE="$(AUTOMAKE)" GTKDOCIZE=true $(HOST_DIR)/bin/autoreconf -f -i'

if [ ! -d "${source_dir}" ]; then
  echo "Missing Buildroot source: ${source_dir}" >&2
  echo "Fetch it first: examples/linux/buildroot/fetch.sh" >&2
  exit 1
fi

if ! command -v "${make_cmd}" >/dev/null 2>&1; then
  echo "Missing GNU Make executable: ${make_cmd}" >&2
  exit 1
fi

if ! command -v "${hostcc}" >/dev/null 2>&1; then
  echo "Missing host C compiler: ${hostcc}" >&2
  exit 1
fi

if ! command -v "${hostcxx}" >/dev/null 2>&1; then
  echo "Missing host C++ compiler: ${hostcxx}" >&2
  exit 1
fi

if [ -z "${bash_cmd}" ] || ! command -v "${bash_cmd}" >/dev/null 2>&1 || ! "${bash_cmd}" -c 'type mapfile >/dev/null 2>&1'; then
  bash_cmd="bash"
fi

if ! command -v "${bash_cmd}" >/dev/null 2>&1; then
  echo "Missing Bash executable: ${bash_cmd}" >&2
  exit 1
fi

if ! "${bash_cmd}" -c 'type mapfile >/dev/null 2>&1'; then
  echo "Bash executable does not support mapfile: ${bash_cmd}" >&2
  echo "Install a newer Bash and ensure it appears before older system shells in PATH." >&2
  exit 1
fi

if ! command -v patch >/dev/null 2>&1 || ! patch --version 2>/dev/null | grep -q "GNU patch"; then
  echo "Missing GNU patch." >&2
  exit 1
fi

if ! command -v flock >/dev/null 2>&1; then
  echo "Missing flock." >&2
  exit 1
fi

# First materialize the Harbor-owned defconfig into Buildroot's output .config.
"${make_cmd}" -C "${source_dir}" O="${output_dir}" BR2_DEFCONFIG="${defconfig}" HOSTCC="${hostcc}" HOSTCXX="${hostcxx}" BASH="${bash_cmd}" AUTORECONF="${autoreconf_cmd}" defconfig

# Then build the kernel, toolchain, BusyBox userspace, and root filesystem.
"${make_cmd}" -C "${source_dir}" O="${output_dir}" HOSTCC="${hostcc}" HOSTCXX="${hostcxx}" BASH="${bash_cmd}" AUTORECONF="${autoreconf_cmd}"

echo "Buildroot output: ${output_dir}"
