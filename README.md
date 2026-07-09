# WiiOS

A small, reproducible homebrew shell/launcher for the Nintendo Wii. Built on
Broadway over IOS with `devkitPPC` + `libogc`, distributed through the Homebrew
Channel.

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Platform: Wii](https://img.shields.io/badge/platform-Wii-green.svg)]()
[![Status: v0.1.x](https://img.shields.io/badge/status-v0.1.x-orange.svg)]()

---

## What Is It

WiiOS is a small homebrew shell for the Nintendo Wii that boots through the
Homebrew Channel as a `.dol` and provides a launcher/desktop surface over IOS.
The project is intentionally minimal and reproducible: there is no custom
channel, no WAD, no NAND-level installation, just a single binary that boots,
resolves storage from SD/USB/FAT-style device roots, reads `config.ini`, and runs
packaged/built-in apps.

It is built on `devkitPPC` + `libogc` and is meant to be a tiny, easy-to-fork
base for homebrew experiments on Wii portables and stock hardware.

This repository is **not** currently a Wii Linux distro. The current codebase is
a Broadway/IOS homebrew runtime and shell.

## Status

The repository currently represents the post-`v0.1.0` / `v0.1.x` base. Runtime
logs still identify the build as `v0.1.0-dev`; the next milestone is `v0.2.0`
(see `docs/ROADMAP_v0.2-v0.3.md`).

Implemented in the current tree:

- HBC boot with a reproducible package layout.
- Stable VI/XFB render surface.
- PAD/WPAD input wired through libogc.
- Explicit storage resolution across app package roots and WiiOS data roots.
- SD-first behavior with USB/FAT-style fallbacks where the runtime exposes them.
- Atomic `boot_to` persistence in `config.ini`.
- Launcher boot mode with `HELLO`, `FILES`, and `SETTINGS` tiles.
- Desktop demo boot mode with a basic window manager/compositor surface.
- Built-in Settings app for toggling launcher/desktop boot mode.
- Built-in File Manager app that lists the active `apps/` directory.
- Fixed external `hello` app manifest loading from `apps/hello/manifest.ini`.
- In-memory ring log with stdout dump on shutdown.
- Explicit handling for missing config, invalid config, missing/invalid manifest,
  and storage initialization failure.

Not implemented yet:

- Dynamic app registry scanning for arbitrary `apps/*/manifest.ini` entries.
- General app-level persistence helpers under `data/apps/<app_id>/`.
- Persistent disk logging such as `logs/wiios.log`.
- Full release-check automation and artifact hashing.
- Custom channel / WAD distribution.
- Priiloader integration.
- Title-style NAND save access.

## Features

### Runtime

- Broadway/IOS runtime using `libogc`.
- VI/XFB render pipeline.
- PAD/WPAD input mapped to high-level actions.
- FAT-backed filesystem services through the IOS backend.
- Service manager abstraction with IOS/null backend separation.

### Storage and config

`make package` produces an HBC package rooted at `dist/apps/wiios/`. On real
hardware this is normally copied to:

```text
sd:/apps/wiios/
```

The storage resolver can also use equivalent device roots when available, such
as `sd0:/apps/wiios`, `usb:/apps/wiios`, `fat:/apps/wiios`, and data-style roots
such as `sd:/wiios` or `usb:/wiios`.

The active root contains:

```text
config.ini
apps/hello/manifest.ini
```

`config.ini` controls the boot mode:

```ini
[boot]
boot_to=launcher
```

or:

```ini
[boot]
boot_to=desktop
```

Settings writes are merged and saved through a temporary file followed by a
rename to avoid half-written config state.

### Apps and shell modes

Current app model:

- `launcher_app`: built-in launcher shell with three tiles.
- `filemgr_app`: built-in read-only view of the active `apps/` directory.
- `settings_app`: built-in boot mode toggle.
- `hello_app`: fixed external demo app loaded only when its manifest is valid.
- `desktop_shell`: demo desktop mode using the window manager/compositor.
- `logviewer_app`: source exists, but persistent disk logging is not wired yet.

The app loader is manifest-aware but not a dynamic registry yet. At the moment,
only `id=hello` is accepted by the loader.

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

```text
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
4. Open `FILES` and confirm the active `apps/` directory is listed.
5. Open `SETTINGS`, toggle `boot_to`, restart, and confirm persistence.
6. Remove or corrupt `apps/hello/manifest.ini` in a test copy and confirm the
   runtime shows a status message instead of hanging.

### On a real Wii

1. Copy `dist/apps/wiios/` to `sd:/apps/wiios/`.
2. Launch from the Homebrew Channel.
3. Validate render, input, file listing, settings toggle, and `boot_to`
   persistence.
4. Confirm clean exit with `HOME` on Wiimote or `START` on GameCube controller.

## Controls

| Input | Action |
|---|---|
| `A` | Accept / open / toggle |
| `B` | Back. In desktop mode, saves `boot_to=launcher` |
| `LEFT` / `RIGHT` | Move focus / selection |
| `HOME` (Wiimote) / `START` (GC) | Exit |

## Project Structure

- `broadwayos/`: Wii runtime — core, video, input, services, apps, shell.
- `shared/`: shared types, event definitions, INI parser, manifest parser,
  ring log.
- `assets/`: HBC icon, internal fonts, internal icons, metadata.
- `sdroot_template/`: files copied into the HBC package.
- `docs/`: architecture, quickstart, smoke tests, known limits, roadmap.

## Documentation

- [Quickstart](docs/QUICKSTART.md)
- [Architecture](docs/ARCH.md)
- [Smoke test v0.1.x](docs/SMOKE_TEST_v0.1.0.md)
- [Known limits](docs/KNOWN_LIMITS.md)
- [Roadmap v0.2 - v0.3](docs/ROADMAP_v0.2-v0.3.md)
- [SPEC v0.2 — App Registry](docs/SPEC_v0.2_APP_REGISTRY.md)

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
