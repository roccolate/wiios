# Known Limits (v0.1.x)

This file tracks current limitations in the repository. It should be read as a
snapshot of the current Broadway/IOS homebrew shell, not as a list of permanent
project constraints.

## App model

- No dynamic app catalog yet.
- The loader reads a manifest, but only the fixed external `hello` app is
  accepted.
- Arbitrary `apps/*/manifest.ini` scanning is planned for `v0.2.0`.
- No dynamic loading of arbitrary external binaries.
- `filemgr_app` is read-only and currently lists the active `<root>/apps`
  directory only.
- `settings_app` only toggles `boot_to=launcher` / `boot_to=desktop`.
- `desktop_shell` is a demo surface, not a production desktop.
- `logviewer_app` source exists, but persistent disk logging is not wired yet.

## Persistence and logging

- General app-level persistence helpers are not implemented yet.
- `data/apps/<app_id>/state.ini` is planned but not available.
- Runtime logging is ring-buffer/in-memory with stdout dump on shutdown.
- No persistent `logs/wiios.log` / `logs/wiios.prev.log` rotation yet.
- `config.ini` persistence exists and is saved atomically through a temp file and
  rename.

## Storage

- Storage depends on libogc/fat-backed device availability.
- Resolver supports several SD/USB/FAT-style root names, but real availability
  depends on the runtime/device environment.
- The default real-hardware package location remains `sd:/apps/wiios`.
- Missing config is recoverable; missing storage backend initialization enters
  the storage error screen.

## UX and recovery

- Recovery messages exist for several boot/config/manifest/storage cases, but
  they are still minimal.
- Storage error screen supports retry/exit/timeout, but no detailed diagnostic
  page yet.
- No full troubleshooting/runbook workflow yet.

## Release engineering

- No automated end-to-end test pipeline.
- No `release-check` target yet.
- No generated SHA256 artifact manifest yet.
- Build artifacts are intentionally not versioned.

## Out of scope for the current tree

- Custom channel / WAD distribution.
- Priiloader integration.
- Title-style NAND save access.
- Wii Linux distro behavior.
- Real browser stack.
- Native Wii GPU driver work beyond the current libogc/VI/XFB homebrew path.
