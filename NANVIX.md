# OpenBLAS Port for Nanvix

> **TL;DR:** This is a port of the OpenBLAS linear algebra library for the Nanvix operating system. Jump to [Quick Start](#quick-start) to get started immediately.

---

## Overview

This document describes the port of [OpenBLAS](https://www.openblas.net/) linear algebra library for the [Nanvix](https://github.com/nanvix/nanvix) operating system. This port enables OpenBLAS to run on Nanvix, a POSIX-compatible educational operating system.

| Property | Value |
|----------|-------|
| **Base Version** | OpenBLAS 0.3.29 |
| **Target Platform** | Nanvix (i686) |
| **Build System** | GNU Make |

**What's included:**
- ✅ Cross-compilation support for Nanvix
- ✅ Static library build (`libopenblas.a`)
- ✅ CBLAS interface
- ✅ Build helper scripts
- ✅ CI/CD integration

**Build Configuration:**
- Target: Pentium II (P2) - i686 compatible
- Threading: Disabled (single-threaded)
- OpenMP: Disabled
- LAPACK: Disabled (BLAS only)
- Fortran: Not required (C interface only)

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [Prerequisites](#prerequisites)
3. [Building](#building)
4. [Testing](#testing)
5. [Changes Summary](#changes-summary)
6. [Known Limitations](#known-limitations)
7. [CI/CD](#cicd)

---

## Quick Start

For experienced users who want to build quickly:

```bash
# 1. Pull the Docker image
docker pull nanvix/toolchain:v0.11.x-minimal

# 2. Download Nanvix sysroot
curl -fsSL https://raw.githubusercontent.com/nanvix/nanvix/refs/heads/dev/scripts/get-nanvix.sh | bash -s -- nanvix-artifacts
tar -xjf nanvix-artifacts/*microvm*single*.tar.bz2 -C nanvix-artifacts
export NANVIX_HOME=$(find nanvix-artifacts -maxdepth 2 -type d -name "bin" -exec dirname {} \; | head -1)

# 3. Build (Docker is used automatically if native toolchain is not found)
make -f Makefile.nanvix CONFIG_NANVIX=y NANVIX_HOME="$NANVIX_HOME"

# 4. Run tests
make -f Makefile.nanvix CONFIG_NANVIX=y NANVIX_HOME="$NANVIX_HOME" test
```

Continue reading for detailed instructions.

---

## Prerequisites

You need two components to build OpenBLAS for Nanvix:

| Component | Description | Default Location |
|-----------|-------------|------------------|
| **Nanvix Toolchain** | i686-nanvix cross-compiler | `$HOME/toolchain` |
| **Nanvix Sysroot** | System libraries and linker script | `$HOME/nanvix` |

### Available Platform Configurations

| Platform | Process Mode | Artifact Pattern |
|----------|--------------|------------------|
| hyperlight | multi-process | `hyperlight.*multi-process` |
| hyperlight | single-process | `hyperlight.*single-process` |
| microvm | single-process | `microvm.*single-process` |
| microvm | multi-process | `microvm.*multi-process` |

### Downloading Nanvix

```bash
curl -fsSL https://raw.githubusercontent.com/nanvix/nanvix/refs/heads/dev/scripts/get-nanvix.sh | bash -s -- nanvix-artifacts
```

The script downloads all release artifacts. Extract the one matching your target platform (see [Quick Start](#quick-start) for a complete example).

---

## Building

### Using Docker (Recommended)

The Makefile supports automatic Docker fallback when the native toolchain is not available:

```bash
# Pull the Nanvix toolchain Docker image
docker pull nanvix/toolchain:v0.11.x-minimal

# Build (Docker is used automatically if native toolchain is not found)
make -f Makefile.nanvix CONFIG_NANVIX=y NANVIX_HOME=/path/to/nanvix/sysroot-debug
```

> **Note:** The sysroot (`NANVIX_HOME`) must contain `lib/libposix.a` and `lib/user.ld` from a Nanvix build.

**Docker Fallback Behavior:**
- If `NANVIX_TOOLCHAIN` points to a valid toolchain, it uses the native compiler
- If the native toolchain is not found, it automatically uses Docker if available
- Use `CONFIG_NANVIX_DOCKER=y` to force Docker usage even when native toolchain exists
- Use `NANVIX_DOCKER_IMAGE` to specify a custom Docker image (default: `nanvix/toolchain:v0.11.x-minimal`)

### Using Native Toolchain

```bash
export NANVIX_TOOLCHAIN=/path/to/toolchain  # Contains: bin/i686-nanvix-gcc
export NANVIX_HOME=/path/to/nanvix          # Contains: lib/user.ld, lib/libposix.a
make -f Makefile.nanvix CONFIG_NANVIX=y all
```

### Build Outputs

After a successful build, you will have:

| File | Description |
|------|-------------|
| `libopenblas.a` | OpenBLAS static library (CBLAS interface) |
| `cblas_test.elf` | CBLAS functional test executable (built by `test` target) |

---

## Testing

### Running the Test Suite

The test target performs both static library verification and functional testing:

```bash
# Run all tests (static verification + functional tests)
make -f Makefile.nanvix CONFIG_NANVIX=y NANVIX_HOME=/path/to/nanvix test
```

### Test Categories

| Test Type | Target | Description |
|-----------|--------|-------------|
| **Static Verification** | `verify` | Validates library archive and symbols |
| **Functional Tests** | `test` | Runs CBLAS tests via nanvixd |

### CBLAS Functional Tests

The test suite covers:

| BLAS Level | Functions Tested |
|------------|------------------|
| Level 1 (single) | saxpy, sdot, snrm2, sscal, scopy, isamax |
| Level 2 (single) | sgemv (matrix-vector multiply) |
| Level 1 (double) | daxpy |

> **Note:** Level 3 BLAS tests (sgemm, dgemm) are skipped for P2/P5/P6 targets because the minimal x86 kernel doesn't include the required copy routines.

### Manual Test Execution

If `nanvixd` is not in the default location, specify it explicitly:

```bash
make -f Makefile.nanvix CONFIG_NANVIX=y NANVIX_HOME=/path/to/nanvix \
     NANVIXD=/path/to/nanvixd test
```

---

## Changes Summary

The following changes were made to support Nanvix.

### Build System Changes

| Change | Description |
|--------|-------------|
| New Makefile | Added `Makefile.nanvix` for Nanvix cross-compilation |
| Cross-compilation | Uses `CONFIG_NANVIX=y` option to enable Nanvix build |
| Docker support | Automatic Docker fallback when native toolchain not available |
| Target CPU | Uses `TARGET=P2` (Pentium II) for i686 compatibility |
| Threading | Disabled (`USE_THREAD=0`, `USE_OPENMP=0`) |
| LAPACK | Disabled (`NO_LAPACK=1`) for smaller build |
| Fortran | Not required (`NO_FORTRAN=1`, `ONLY_CBLAS=1`) |

### Build Options

| Option | Value | Description |
|--------|-------|-------------|
| `TARGET` | P2 | Pentium II target (i686 compatible) |
| `BINARY` | 32 | 32-bit build |
| `CROSS` | 1 | Cross-compilation mode |
| `NO_SHARED` | 1 | Static library only |
| `USE_OPENMP` | 0 | Disable OpenMP |
| `USE_THREAD` | 0 | Disable threading |
| `USE_LOCKING` | 1 | Enable locking for thread safety |
| `USE_TLS` | 0 | Disable thread-local storage |
| `NO_LAPACK` | 1 | Disable LAPACK |
| `NO_FORTRAN` | 1 | No Fortran compiler required |
| `ONLY_CBLAS` | 1 | Build only CBLAS interface |

### New Files

| File | Purpose |
|------|---------|
| `Makefile.nanvix` | Standalone Makefile for Nanvix cross-compilation |
| `NANVIX.md` | This documentation file |
| `.github/workflows/nanvix-ci.yml` | CI workflow for automated builds |

---

## Known Limitations

| Limitation | Impact |
|------------|--------|
| **No shared libraries** | Only static library (`libopenblas.a`) is built |
| **Single-threaded** | Threading support is disabled |
| **No OpenMP** | OpenMP parallelization is not available |
| **No LAPACK** | Only BLAS routines are available |
| **No Fortran interface** | Only CBLAS (C interface) is provided |
| **Limited CPU features** | Targets Pentium II for maximum compatibility |

---

## CI/CD

The GitHub Actions workflow at `.github/workflows/nanvix-ci.yml` automates building and testing on every change.

### Trigger Events

| Event | Description |
|-------|-------------|
| Push to `nanvix/**` | Any push to Nanvix branches |
| PR to `nanvix/**` | Pull requests targeting Nanvix branches |
| Daily schedule | Runs at midnight UTC |
| Manual dispatch | Can be triggered manually |
| Repository dispatch | Triggered by `nanvix-minor-release` or `nanvix-major-release` events |

### Build Matrix

The workflow builds for all platform/process-mode combinations:
- `hyperlight` / `multi-process`
- `hyperlight` / `single-process`
- `microvm` / `multi-process`
- `microvm` / `single-process`

### Release Artifacts

On successful builds (non-PR), artifacts are packaged and released:

| Artifact | Contents |
|----------|----------|
| `openblas-{platform}-{process-mode}.tar.bz2` | `lib/libopenblas.a`, `include/cblas.h`, `include/openblas_config.h` |
