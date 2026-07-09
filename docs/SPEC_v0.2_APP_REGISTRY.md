# SPEC v0.2 — App Registry

## Purpose

Replace the current fixed `hello`-only external app path with a small dynamic app
registry while keeping the Homebrew Channel package layout simple and safe.

This spec documents the intended `v0.2.0` behavior. It should be implemented in
small steps without breaking the existing `apps/hello/manifest.ini` demo.

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

## Non-goals for v0.2

- Dynamic loading of arbitrary external `.dol` / `.elf` binaries.
- NAND title-style app installation.
- WAD/channel behavior.
- Network app catalog.
- Full desktop environment.

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

## Registry entry model

Suggested C struct shape:

```c
typedef enum {
  WII_APP_REG_VALID = 0,
  WII_APP_REG_INVALID_MANIFEST,
  WII_APP_REG_DUPLICATE_ID,
  WII_APP_REG_UNSUPPORTED_KIND,
  WII_APP_REG_ENTRY_MISSING
} WiiAppRegistryStatus;

typedef struct {
  WiiManifest manifest;
  WiiAppRegistryStatus status;
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
   - append registry entry.
5. If registry is empty, launcher should show a clear empty-state message.

## Failure behavior

Bad manifests must not crash the runtime.

Expected UI behavior:

- valid app: normal tile/list entry,
- invalid app: disabled/error entry with a short reason,
- duplicate ID: disabled/error entry,
- missing manifest: disabled/error entry or omitted with log entry,
- empty catalog: show `NO APPS FOUND` or equivalent.

Expected log behavior:

- log root used,
- log each manifest path scanned,
- log parse/validation errors,
- log duplicate IDs.

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
6. Update smoke tests.
7. Update README and ARCH again after implementation.

## Acceptance criteria

- `make clean && make && make package` succeeds.
- Existing `hello` manifest still launches.
- Adding a second valid manifest makes it appear in the launcher.
- Invalid manifest appears as a recoverable error entry or clear status message.
- Duplicate IDs do not crash and are reported.
- Launcher navigation works with 0, 1, 2, and many registry entries.
- No app can escape `<root>/apps/<app_id>/` through path traversal.
