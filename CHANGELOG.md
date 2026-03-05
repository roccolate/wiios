# Changelog

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
