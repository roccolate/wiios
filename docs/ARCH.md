# Arquitectura MVP (v0.0.1)

## Estado actual
- Runtime Broadway sobre IOS usando `libogc`.
- Video por VI/XFB (`surface` en YUYV422).
- Input PAD/WPAD.
- Config y manifests en formato INI.
- FS inicial sobre FAT con raíz `sd:/wiios`.
- Fallback robusto SD/USB planificado para siguiente ciclo.

## Módulos
- `shared/`: tipos, eventos, servicios, parser INI, parser de manifest.
- `broadwayos/core`: tiempo, log, input, cola de eventos.
- `broadwayos/gfx`: init VI + surface + font bitmap.
- `broadwayos/wm`: window manager y compositor.
- `broadwayos/shell`: `desktop_shell`.
- `broadwayos/services`: service manager + backend FAT.
- `broadwayos/apps`: launcher, settings, filemgr, hello.

## Flujo de arranque
1. `main` inicializa video y servicios.
2. Carga config (`config.ini`) para boot mode.
3. Inicializa shell o launcher.
4. Loop principal: input -> eventos -> update/draw -> present.
5. Salida limpia por `HOME`/`START`.
