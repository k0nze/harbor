#!/usr/bin/env sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../../.." && pwd)

# Keep the Buildroot version in a small file so the example is reproducible.
version="${BUILDROOT_VERSION:-$(cat "${repo_root}/examples/linux/buildroot/buildroot.version")}"

# Store downloaded and extracted Buildroot sources outside Git-tracked paths.
download_dir="${repo_root}/.buildroot/downloads"
source_dir="${BUILDROOT_SOURCE_DIR:-${repo_root}/.buildroot/buildroot-${version}}"
archive="${download_dir}/buildroot-${version}.tar.xz"
url="https://buildroot.org/downloads/buildroot-${version}.tar.xz"

if [ -d "${source_dir}" ]; then
  echo "Buildroot already available: ${source_dir}"
  exit 0
fi

mkdir -p "${download_dir}" "${repo_root}/.buildroot"

# Download the pinned release tarball only once.
if [ ! -f "${archive}" ]; then
  echo "Downloading ${url}"
  curl --location "${url}" --output "${archive}"
fi

# Extract Buildroot into .buildroot/ so Harbor does not vendor the source tree.
echo "Extracting ${archive}"
tar -C "${repo_root}/.buildroot" -xf "${archive}"

echo "Buildroot source: ${source_dir}"
