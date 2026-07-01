# Retrocore Spec Alignment

WiiOS aligns its app manifests with `~/retrocore-spec/contracts/app-manifest.md`.

## Current Mapping

| retrocore field | WiiOS field | Notes |
|---|---|---|
| `id` | `id` | Stable app id. |
| `title` | `title` | Canonical display name. Legacy alias: `name`. |
| `kind` | `kind` | Canonical app kind. Legacy alias: `type`. |
| `entry` | `entry` | Wii-local entry path, currently validated for shape. |
| `icon` | `icon` | Wii-local icon path/id. |
| `version` | `version` | App version. |
| `requires` | `requires` | Comma-separated capabilities. |

## Compatibility

The parser accepts both the retrocore canonical fields and the old WiiOS v0.1 aliases:

```ini
[app]
id=hello
title=Hello App
kind=demo_app
entry=app.elf
icon=icon.bin
version=0.1.0
requires=input,storage,window

# Legacy aliases
name=Hello App
type=windowed
```

When only legacy fields are present, WiiOS fills canonical `title`/`kind`. When only canonical fields are present, WiiOS fills legacy `name`/`type` so old internal code keeps working during migration.

## Boundary

WiiOS should consume retrocore contracts as validation/design guidance. It should not import a shared runtime. libogc, PAD/WPAD, SD/USB storage, and Homebrew Channel packaging stay Wii-native.
