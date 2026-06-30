# WiiOS

A small, reproducible homebrew shell/launcher for the Nintendo Wii. Built on
Broadway over IOS with `devkitPPC` + `libogc`, distributed through the Homebrew
Channel.

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Platform: Wii](https://img.shields.io/badge/platform-Wii-green.svg)]()
[![Status: v0.1.0](https://img.shields.io/badge/status-v0.1.0-orange.svg)]()

---

## What Is It

WiiOS is a small homebrew shell for the Nintendo Wii that boots through the
Homebrew Channel as a `.dol` and provides a launcher surface over IOS. The
project is intentionally minimal and reproducible: there is no custom channel,
no WAD, no NAND-level installation, just a single binary that boots, reads its
config from SD or USB, and runs packaged apps.

It is built on `devkitPPC` + `libogc` and is meant to be a tiny, easy-to-fork
base for homebrew experiments on Wii portables and stock hardware.

## Status

`v0.1.0` is shipped. The next milestone is `v0.2` (see `docs/ROADMAP_v0.2-v0.3.md`).

Done in `v0.1.0`:
- HBC boot with a reproducible package.
- Stable VI/XFB render.
- Real PAD/WPAD input.
- Storage resolution with SD priority and USB fallback for config.
- Atomic `boot_to` persistence in `config.ini`.
- Explicit handling of boot states and manifest errors.

Explicit non-goals for now: custom channel / WAD distribution, Priiloader
integration, title-style NAND access.

## Features

- HBC boot with a reproducible package layout.
- VI/XFB render pipeline.
- PAD/WPAD input wired through libogc.
- SD-first storage with USB fallback for config.
- Atomic `config.ini` writes (no half-flushed state).
- Manifest-driven app registry.
- Apps ship inside `sdroot_template/` and are bundled by `make package`.

## Build

### Requirements

- `devkitPPC`
- `libogc`
- `make`

Default paths (override in your shell if needed):

```bash
DEVKITPRO=/opt/devkitpro
DEVKITPPC=/opt/devkitpro/devkitPPC
```

### Build and package

```bash
make clean && make
make package
```

Expected artefacts after `make package`:

```
dist/apps/wiios/boot.dol
dist/apps/wiios/meta.xml
dist/apps/wiios/icon.png
dist/apps/wiios/config.ini
dist/apps/wiios/apps/hello/manifest.ini
```

## Test

### On Dolphin

1. Open `dist/apps/wiios/boot.dol` in Dolphin.
2. Validate render.
3. Validate controls.
4. Change `boot_to` and confirm it persists after restart.

### On a real Wii

1. Copy `dist/apps/wiios/` to `sd:/apps/wiios/`.
2. Launch from the Homebrew Channel.
3. Validate render, input, and `boot_to` persistence.

## Controls

| Input | Action |
|---|---|
| `A` | Accept |
| `B` | Back |
| `LEFT` / `RIGHT` | Move focus / selection |
| `HOME` (Wiimote) / `START` (GC) | Exit |

## Project Structure

- `broadwayos/`: Wii runtime — core, video, input, services, apps, shell.
- `shared/`: shared types, event definitions, INI parser, manifest parser.
- `assets/`: HBC icon, internal fonts, internal icons, metadata.
- `sdroot_template/`: files copied into the HBC package.
- `docs/`: architecture, quickstart, smoke tests, known limits, roadmap.

## Documentation

- [Quickstart](docs/QUICKSTART.md)
- [Architecture](docs/ARCH.md)
- [Smoke test v0.1.0](docs/SMOKE_TEST_v0.1.0.md)
- [Known limits](docs/KNOWN_LIMITS.md)
- [Roadmap v0.2 - v0.3](docs/ROADMAP_v0.2-v0.3.md)

## Build artefacts (not versioned)

The `build/` and `dist/` directories plus packaged binaries (`*.dol`, `*.elf`,
`*.rar`, `*.zip`, `*.7z`) are not versioned. To publish a reproducible build,
run `make package` and attach the resulting `dist/apps/wiios/` bundle to a
GitHub Release.

## License

MIT — see [LICENSE](LICENSE).

## Acknowledgements

- The Wii homebrew scene and the `devkitPro` toolchain maintainers.
- `libogc` for the Broadway / IOS host environment.
- The RetroArch / HBC ecosystem for the packaging conventions.