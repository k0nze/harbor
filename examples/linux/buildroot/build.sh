#!/usr/bin/env sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../../.." && pwd)
docker_cmd="${DOCKER:-docker}"
image_name="${HARBOR_EXAMPLES_DOCKER_IMAGE:-harbor-examples-builder:latest}"
source_volume="${HARBOR_BUILDROOT_SOURCE_VOLUME:-harbor-buildroot-source}"
output_volume="${HARBOR_BUILDROOT_OUTPUT_VOLUME:-harbor-buildroot-output}"
in_container="${HARBOR_IN_DOCKER:-0}"

if [ -n "${BUILDROOT_OUTPUT_DIR:-}" ]; then
  case "${BUILDROOT_OUTPUT_DIR}" in
    /*) host_output_dir="${BUILDROOT_OUTPUT_DIR}" ;;
    *) host_output_dir="${repo_root}/${BUILDROOT_OUTPUT_DIR}" ;;
  esac
else
  host_output_dir="${repo_root}/build/buildroot/riscv64-qemu-virt"
fi

if [ "${in_container}" != "1" ]; then
  if ! command -v "${docker_cmd}" >/dev/null 2>&1; then
    echo "Docker executable not found: ${docker_cmd}" >&2
    exit 1
  fi

  if ! "${docker_cmd}" info >/dev/null 2>&1; then
    echo "Docker daemon is not reachable. Start Docker Desktop and try again." >&2
    exit 1
  fi

  if ! "${docker_cmd}" image inspect "${image_name}" >/dev/null 2>&1; then
    echo "Missing Docker image: ${image_name}" >&2
    echo "Build it first with the command documented in README.md." >&2
    exit 1
  fi

  mkdir -p "${host_output_dir}"

  # Keep Buildroot sources and output in Docker volumes. Only final images are
  # copied back to the host build directory.
  echo "[buildroot] Preparing Docker volumes"
  "${docker_cmd}" run --rm \
    -v "${source_volume}:/buildroot-source" \
    -v "${output_volume}:/buildroot-output" \
    "${image_name}" \
    sh -c "mkdir -p /buildroot-source /buildroot-output && chown $(id -u):$(id -g) /buildroot-source /buildroot-output"

  echo "[buildroot] Building in Docker image ${image_name}"
  "${docker_cmd}" run --rm \
    -u "$(id -u):$(id -g)" \
    -v "${repo_root}:/work:ro" \
    -v "${source_volume}:/buildroot-source" \
    -v "${output_volume}:/buildroot-output" \
    -v "${host_output_dir}:/host-buildroot" \
    -w /work \
    -e HARBOR_IN_DOCKER=1 \
    -e BUILDROOT_VERSION="${BUILDROOT_VERSION:-}" \
    -e BUILDROOT_SOURCE_DIR="${BUILDROOT_SOURCE_DIR:-}" \
    -e MAKE \
    -e HOSTCC \
    -e HOSTCXX \
    -e BASH \
    "${image_name}" \
    sh /work/examples/linux/buildroot/build.sh

  exit $?
fi

# Use the same pinned Buildroot version as fetch.sh unless the user overrides it.
version="${BUILDROOT_VERSION:-$(cat "${repo_root}/examples/linux/buildroot/buildroot.version")}"

# Buildroot supports out-of-tree builds; keep all generated files under build/.
source_dir="${BUILDROOT_SOURCE_DIR:-/buildroot-source/buildroot-${version}}"
output_dir="/buildroot-output/riscv64-qemu-virt"
defconfig="${repo_root}/examples/linux/buildroot/riscv64_qemu_virt_defconfig"
external_dir="${repo_root}/examples/linux/buildroot/external"

# Tool setup is handled by the shared example container. These overrides are
# useful if a user wants a non-default GNU Make or host compiler inside it.
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

echo "[buildroot] Configuring Buildroot"
# First materialize the Harbor-owned defconfig into Buildroot's output .config.
"${make_cmd}" -C "${source_dir}" O="${output_dir}" BR2_EXTERNAL="${external_dir}" BR2_DEFCONFIG="${defconfig}" HOSTCC="${hostcc}" HOSTCXX="${hostcxx}" BASH="${bash_cmd}" AUTORECONF="${autoreconf_cmd}" defconfig

echo "[buildroot] Rebuilding Harbor mmio-test package"
# mmio-test is a local Buildroot package; force its package directory to be
# refreshed so source changes in this repository are reflected in the rootfs.
"${make_cmd}" -C "${source_dir}" O="${output_dir}" BR2_EXTERNAL="${external_dir}" HOSTCC="${hostcc}" HOSTCXX="${hostcxx}" BASH="${bash_cmd}" AUTORECONF="${autoreconf_cmd}" mmio-test-dirclean

echo "[buildroot] Building kernel, rootfs, and userspace"
# Then build the kernel, toolchain, BusyBox userspace, and root filesystem.
"${make_cmd}" -C "${source_dir}" O="${output_dir}" BR2_EXTERNAL="${external_dir}" HOSTCC="${hostcc}" HOSTCXX="${hostcxx}" BASH="${bash_cmd}" AUTORECONF="${autoreconf_cmd}"

echo "[buildroot] Copying final images to host output directory"
mkdir -p /host-buildroot/images
cp "${output_dir}/images/Image" "${output_dir}/images/rootfs.cpio" /host-buildroot/images/

echo "Buildroot output: ${output_dir}"
