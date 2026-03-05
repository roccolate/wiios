# WiiOS

Scaffold inicial para un OS tipo shell/launcher en Broadway sobre IOS.

## Estado
- MS1 (Semana 1) implementado: `core/`, `gfx/`, `ui/` base y `logviewer_app`.
- MS2 (Semana 2) implementado: `wm/`, `desktop_shell` y 2 ventanas demo con foco.
- MS3 (Semana 3) implementado: `services/` + backend FAT (`sd:/`) + `app_loader` y carga de app externa `hello` via `manifest.ini`.
- MS4 (Semana 4) implementado: `launcher_app` fullscreen + `filemgr_app` + `settings_app` + `boot_to launcher` por `config.ini`.

## Build (devkitPPC)
```bash
cd wiios
make
# salida principal:
# build/wiios.elf
# build/wiios.dol
```

## Package (HBC)
```bash
cd wiios
make package
```

Salida estándar para copiar a SD:
- `dist/apps/wiios/boot.dol`
- `dist/apps/wiios/meta.xml`
- `dist/apps/wiios/icon.png`
- `dist/apps/wiios/config.ini`
- `dist/apps/wiios/apps/hello/manifest.ini`

## Runtime actual
- `vi.c` usa `libogc` (`VIDEO_Init`, framebuffers reales).
- `surface.c` dibuja rectángulos en `YUYV422` sobre XFB (visible en Dolphin/Wii).
- `font_bitmap.c` renderiza texto 5x7 para títulos y hints de UI.
- input real con `PAD/WPAD`:
  - `A`: `ACT_OK`
  - `B`: `ACT_BACK`
  - `LEFT/RIGHT`: `ACT_PREV` / `ACT_NEXT`
  - `HOME` o `START`: salir del loop principal
