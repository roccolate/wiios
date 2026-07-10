# Retrocore Adoption Notes for WiiOS

WiiOS may reuse ideas from `retrocore-spec`, but it must not become dependent on
a shared runtime. The rule is:

> Adopt contracts and vocabulary. Keep Wii implementation native.

## What WiiOS adopts

### 1. App manifest vocabulary

WiiOS `manifest.ini` files should follow the shared app-manifest vocabulary:

```ini
[app]
id=hello
title=Hello App
kind=demo_app
entry=app.elf
icon=icon.bin
version=0.1.0
requires=input,storage,window
```

Canonical fields:

- `id`
- `title`
- `kind`
- `entry`
- `icon`
- `version`
- `requires`

WiiOS currently keeps legacy aliases during migration:

- `name` -> `title`
- `type` -> `kind`

### 2. App launch failure vocabulary

Use shared failure names where possible:

| WiiOS case | Shared failure |
|---|---|
| Manifest missing | `not_found` |
| Manifest parse/validation failed | `invalid_manifest` |
| Duplicate app id | `invalid_manifest` or `launch_failed` with detail |
| Unsupported app kind | `unsupported_platform` |
| Required capability unavailable | `missing_capability` |
| App could not start | `launch_failed` |
| Access denied by policy | `permission_denied` |

The user-facing string may stay Wii-specific, but logs and app registry state
should use this vocabulary.

### 3. Logical event vocabulary

Raw Wii input stays local. PAD/WPAD constants should never leak into retrocore.

Suggested WiiOS mapping:

| WiiOS input/action | Logical meaning |
|---|---|
| `A` | activate / launch selected app |
| `B` | back / close focused window or panel |
| `LEFT` / `RIGHT` | focus previous / focus next |
| `HOME` / `START` | system exit |

Future registry/desktop code may use names like:

- `launch_app`
- `focus_next`
- `focus_prev`
- `close_window`
- `menu_action`

### 4. Window model vocabulary

WiiOS should use the shared words but keep a Wii-friendly implementation.

Useful concepts:

- window
- focused window
- z-order
- modal
- minimized
- maximized

Allowed WiiOS adaptation:

- panels instead of overlapping windows,
- cards instead of desktop windows,
- launcher focus instead of pointer focus,
- no free dragging unless it becomes useful on Wii.

### 5. Theme tokens later

Theme tokens can be adopted after the app registry and desktop behavior stabilize.
WiiOS may quantize or reinterpret any theme for the VI/XFB renderer.

## What WiiOS must not adopt

Do not import or create:

- shared renderer,
- shared runtime,
- common framework,
- kernel abstraction layer,
- platform-hiding filesystem layer,
- non-Wii input model,
- assumptions about POSIX, mouse, keyboard, or terminal behavior.

Keep these Wii-native:

- `libogc` boot/input/render/storage glue,
- Homebrew Channel package layout,
- PAD/WPAD constants,
- SD/USB/FAT path resolution,
- `.dol` build/package flow,
- VI/XFB rendering details.

## Near-term implementation impact

For `v0.2.0`, this means:

1. `docs/SPEC_v0.2_APP_REGISTRY.md` should treat retrocore app-manifest fields
   as canonical.
2. The app registry should expose structured status/failure values.
3. The launcher should become data-driven, but the execution mapping may remain
   Wii-local and fixed, e.g. `id=hello` -> `hello_app_api()`.
4. Desktop/window work should use the window-model vocabulary without promising a
   full overlapping desktop.

## Maintenance rule

When WiiOS adopts or diverges from a retrocore contract:

1. Update this file.
2. Update the matching WiiOS implementation/spec docs.
3. Update `retrocore-spec/adapters/wiios.md`.
4. Update `retrocore-spec/contracts/compliance.md` if adoption status changes.
