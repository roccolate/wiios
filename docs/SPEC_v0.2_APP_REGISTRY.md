# SPEC v0.2 — App Registry

## Purpose

Replace the current fixed `hello`-only external app path with a small dynamic app
registry while keeping the Homebrew Channel package layout simple and safe.

This spec documents the intended `v0.2.0` behavior. It should be implemented in
small steps without breaking the existing `apps/hello/manifest.ini` demo.

## Retrocore alignment

The app registry should follow the shared `retrocore-spec` app-manifest
vocabulary where it helps, without depending on any shared runtime.

Adopt from retrocore:

- canonical manifest field names,
- app capability vocabulary,
- launch/failure vocabulary,
- logical event names where useful for tests and docs.

Do not adopt from retrocore:

- runtime code,
- renderer code,
- platform abstraction layers,
- assumptions about POSIX, terminal UIs, keyboard/mouse, or non-Wii storage.

WiiOS remains native to:

- `libogc`,
- Homebrew Channel package layout,
- PAD/WPAD input,
- VI/XFB rendering,
- SD/USB/FAT-style Wii paths,
- `.dol` build/package flow.

See also:

- `docs/RETROCORE_ADOPTION.md`
- `retrocore-spec/contracts/app-manifest.md`
- `retrocore-spec/adapters/wiios.md`

## Current baseline

Current behavior:

- `main.c` searches known roots and tries to load one fixed manifest:

```text
<root>/apps/hello/manifest.ini
```

- `app_loader.c` parses the manifest, validates basic shape, and accepts only:

```ini
[app]
id=hello
```

- The launcher has fixed tiles:

```text
HELLO   FILES   SETTINGS
```

- Built-in apps are compiled into `boot.dol`.
- External app loading is not truly dynamic yet.

## Goals

- Scan all app manifests under `<root>/apps/*/manifest.ini`.
- Build a registry of valid and invalid app entries.
- Show apps in the launcher without hardcoding the visual catalog.
- Preserve the `hello` demo app.
- Avoid crashes on bad manifests.
- Avoid path traversal and buffer overflows.
- Emit structured registry/failure status using retrocore-compatible vocabulary
  where practical.

## Non-goals for v0.2

- Dynamic loading of arbitrary external `.dol` / `.elf` binaries.
- NAND title-style app installation.
- WAD/channel behavior.
- Network app catalog.
- Full desktop environment.
- Shared retrocore runtime dependency.

## Package layout

Recommended HBC package root:

```text
sd:/apps/wiios/
```

Runtime root after resolver:

```text
<root>/
  config.ini
  apps/
    hello/
      manifest.ini
    second-demo/
      manifest.ini
  data/
  logs/
```

## Manifest format

Canonical format:

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

Canonical fields follow retrocore vocabulary:

| Field | Required | Meaning |
|---|---:|---|
| `id` | yes | Stable app id. |
| `title` | yes | Human-facing label. |
| `kind` | yes | App kind such as `system_app`, `bundled_app`, `external_app`, `demo_app`, or a WiiOS extension. |
| `entry` | yes | WiiOS-local launch target or future app entry path. |
| `icon` | no | Symbolic icon id or local icon reference. |
| `version` | no | App version string. |
| `requires` | no | Comma-separated capability list. |

Legacy aliases are supported by the current parser:

```ini
name=Hello App
type=windowed
```

Alias rules:

- `title` is canonical display name.
- `name` is legacy alias for `title`.
- `kind` is canonical app kind.
- `type` is legacy alias for `kind`.

## Required fields

A manifest is valid for registry display when it has:

- `[app]`
- `id`
- `title` or `name`
- `kind` or `type`
- `entry`

For v0.2, `entry` does not imply arbitrary dynamic binary loading. It identifies
what the future app entrypoint would be and lets the registry validate/package
metadata consistently.

## ID rules

`id` must:

- be non-empty,
- be unique within the registry,
- fit inside the manifest struct buffer,
- contain only safe characters:
  - lowercase letters,
  - numbers,
  - dash,
  - underscore.

Recommended regex:

```text
^[a-z0-9_-]+$
```

Reject:

- `../evil`
- `/absolute`
- empty IDs,
- IDs with spaces,
- duplicate IDs.

## Capability vocabulary

Use retrocore-compatible capability names where possible:

- `input`
- `storage`
- `window`
- `network`
- `audio`
- `shell`
- `settings`
- `timer`

Wii-specific capabilities should use a prefix:

- `wii:sd`
- `wii:usb`
- `wii:wpad`
- `wii:gcpad`
- `wii:xfb`

## Failure vocabulary

Suggested registry/launch failure values:

| Failure | Use when |
|---|---|
| `not_found` | Manifest or app id is missing. |
| `invalid_manifest` | Manifest exists but fails parsing or validation. |
| `missing_capability` | App requires a capability not available on the current WiiOS runtime. |
| `launch_failed` | App is valid but could not start. |
| `permission_denied` | Policy blocks the app. |
| `unsupported_platform` | Manifest/app kind is valid but unsupported by WiiOS. |

User-facing messages may remain shorter, but logs and registry state should use
these values where practical.

## Registry entry model

Suggested C struct shape:

```c
typedef enum {
  WII_APP_REG_VALID = 0,
  WII_APP_REG_INVALID_MANIFEST,
  WII_APP_REG_DUPLICATE_ID,
  WII_APP_REG_UNSUPPORTED_KIND,
  WII_APP_REG_ENTRY_MISSING,
  WII_APP_REG_MISSING_CAPABILITY
} WiiAppRegistryStatus;

typedef struct {
  WiiManifest manifest;
  WiiAppRegistryStatus status;
  char failure_code[32];
  char manifest_path[160];
  char error[80];
} WiiAppRegistryEntry;
```

The launcher should be able to render both valid and invalid entries.

## Scanning algorithm

1. Resolve active root using `path_resolver_root()`.
2. Build apps path:

```text
<root>/apps
```

3. List immediate child directories.
4. For each child:
   - ignore `.` and `..`,
   - reject names that do not match safe ID/path rules,
   - build path `<root>/apps/<child>/manifest.ini`,
   - parse manifest,
   - validate required fields,
   - check duplicate IDs,
   - validate capabilities if feasible,
   - append registry entry.
5. If registry is empty, launcher should show a clear empty-state message.

## Failure behavior

Bad manifests must not crash the runtime.

Expected UI behavior:

- valid app: normal tile/list entry,
- invalid app: disabled/error entry with a short reason,
- duplicate ID: disabled/error entry,
- missing manifest: disabled/error entry or omitted with log entry,
- unsupported kind: disabled/error entry,
- empty catalog: show `NO APPS FOUND` or equivalent.

Expected log behavior:

- log root used,
- log each manifest path scanned,
- log parse/validation errors,
- log duplicate IDs,
- log unsupported kinds/capabilities.

## Launcher behavior

For v0.2, launcher should become data-driven:

- built-in system entries may still exist:
  - `FILES`,
  - `SETTINGS`,
  - optional `LOGS` once persistent logging exists.
- external/demo apps come from registry.
- navigation should not assume exactly three tiles.
- `LEFT`/`RIGHT` should wrap over the current entry count.
- `A` should open valid entries only.
- `B` should return from sub-app to launcher.

## Desktop/window vocabulary

The registry should not force desktop mode yet, but it should not block it.

When an app eventually opens in desktop mode, use shared vocabulary in docs/tests:

- app launch creates or focuses a window/panel,
- focused window receives actions,
- `focus_next` cycles eligible windows/panels,
- close/back resolves the focused window/panel.

WiiOS may implement panels/cards instead of draggable overlapping windows.

## Compatibility requirement

The existing demo must keep working:

```text
<root>/apps/hello/manifest.ini
```

with:

```ini
[app]
id=hello
title=Hello App
kind=demo_app
entry=app.elf
```

## Suggested implementation order

1. Add `app_registry.h/.c` with scan/validate functions.
2. Add unit-like parser validation where possible with host-buildable helpers if
   practical.
3. Replace fixed launcher tile count with registry-backed list.
4. Keep `FILES` and `SETTINGS` as built-in system entries.
5. Preserve fixed `hello_app_api()` mapping for `id=hello` until true dynamic app
   loading exists.
6. Add failure/status strings using the vocabulary in this spec.
7. Update smoke tests.
8. Update README and ARCH again after implementation.

## Acceptance criteria

- `make clean && make && make package` succeeds.
- Existing `hello` manifest still launches.
- Adding a second valid manifest makes it appear in the launcher.
- Invalid manifest appears as a recoverable error entry or clear status message.
- Duplicate IDs do not crash and are reported.
- Unsupported app kinds/capabilities do not crash and are reported.
- Launcher navigation works with 0, 1, 2, and many registry entries.
- No app can escape `<root>/apps/<app_id>/` through path traversal.
