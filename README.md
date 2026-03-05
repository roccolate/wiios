# WiiOS PoC

WiiOS es un proof-of-concept de shell/launcher para Nintendo Wii (Broadway + IOS), construido como homebrew con `devkitPPC` y `libogc`.

Este tag `v0.0.1` representa el demo original auditable del proyecto.

## Qué funciona hoy
- Arranque desde Homebrew Channel con `boot.dol`.
- Render de UI en pantalla real (VI/XFB).
- Input real (`PAD/WPAD`).
- Dos modos de interfaz: `desktop` y `launcher`.
- Apps demo (`hello`, `filemgr`, `settings`).
- Configuración en `config.ini` y manifest de app en `manifest.ini`.

## Qué NO pretende este PoC
- No instala canal/WAD.
- No implementa guardados tipo título oficial en NAND.
- No integra flujo oficial de Priiloader (queda como feature posterior).
- No se considera release final para usuarios masivos.

## Requisitos
- `devkitPPC`
- `libogc`
- `make`

## Build
```bash
cd wiios
make clean && make
```

Artefactos:
- `build/wiios.elf`
- `build/wiios.dol`

## Package (HBC)
```bash
cd wiios
make package
```

Salida:
- `dist/apps/wiios/boot.dol`
- `dist/apps/wiios/meta.xml`
- `dist/apps/wiios/icon.png`
- `dist/apps/wiios/config.ini`
- `dist/apps/wiios/apps/hello/manifest.ini`

## Probar en Dolphin
1. Abre Dolphin.
2. `File > Open...`
3. Carga `dist/apps/wiios/boot.dol`.
4. Verifica navegación y render.

## Probar en Wii real
1. Copia `dist/apps/wiios` a `sd:/apps/wiios`.
2. Abre Homebrew Channel.
3. Ejecuta `WiiOS`.
4. Verifica render/input y salida con `HOME`.

## Controles actuales
- `A`: aceptar
- `B`: atrás
- `LEFT/RIGHT`: cambiar foco o selección
- `HOME` (Wiimote) / `START` (GC): salir

## Post-v0.0.1 roadmap
- Persistencia completa de settings y estado.
- Fallback SD/USB robusto y validado.
- Hardening de errores + smoke tests más amplios.
