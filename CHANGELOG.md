# Changelog

## v0.1.0 - 2026-03-05

### Added
- `path_resolver` central para rutas con prioridad `sd:/wiios` y fallback `usb:/wiios`.
- Extensión de `WiiServices`/`WiiBackend` con operaciones FS de escritura y utilidades (`write_all`, `exists`, `mkdirs`, `rename`).
- Persistencia atómica de `boot_to` en `config.ini` desde Settings.
- Pantalla explícita de `STORAGE ERROR` cuando falla inicialización de servicios.
- Banner de estado de arranque para `CONFIG MISSING` y errores de manifest.
- `docs/SMOKE_TEST_v0.1.0.md` y release notes de v0.1.0.

### Changed
- Rutas absolutas hardcodeadas migradas a rutas relativas resueltas en runtime.
- File manager ahora diferencia `PATH NOT FOUND`, `I/O ERROR` y directorio vacío.
- Loader valida manifest y devuelve errores tipados para manifest inválido.

### Known Issues
- Catálogo de apps aún no es dinámico (demo `hello` fija).
- Persistencia de estado app-level fuera de settings aún no completa.
- Sin suite automatizada de tests de integración.

## v0.0.1 - 2026-03-05

Release demo / proof-of-concept.

### Added
- Build reproducible con `devkitPPC` + `libogc`.
- Package HBC (`boot.dol`, `meta.xml`, `icon.png`).
- Render UI básico con `desktop` y `launcher`.
- Input real PAD/WPAD.
- Carga de app demo externa (`hello`) por `manifest.ini`.

### Changed
- Configuración migrada a `config.ini`.
- Estructura de proyecto modular (`core`, `gfx`, `wm`, `shell`, `services`, `apps`).

### Known Issues
- Persistencia incompleta en settings (flujo parcial).
- File manager básico (sin navegación completa).
- Sin QA automatizada de regresión.
