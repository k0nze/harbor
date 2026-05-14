# Linux Examples

This directory is for Harbor-owned Linux boot helpers and notes.

The first Linux path should use Buildroot as a controlled baseline for QEMU
`virt` before moving on to real distro images. Generated kernels, root
filesystems, disk images, downloaded sources, and logs should stay out of Git.

Expected future layout:

```text
examples/linux/
  buildroot/
    README.md
    run-riscv64-virt.sh
  distro/
    README.md
```

Use this directory for small scripts and documentation only. Large generated
artifacts belong under `build/`, `.buildroot/`, `images/`, or another ignored
output directory.

