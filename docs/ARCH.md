# Arquitectura MVP (v0.1.0)

## Estado actual
- Runtime Broadway sobre IOS usando `libogc`.
- Video por VI/XFB (`surface` en YUYV422).
- Input PAD/WPAD.
- Config y manifests en formato INI.
- FS sobre FAT con resolución de storage SD primero y fallback USB.
- Persistencia de config con escritura atómica.
- Estados explícitos para arranque, fallback, errores de storage y manifest.

## Módulos
- `shared/`: tipos, eventos, servicios, parser INI y parser de manifest.
- `broadwayos/src/core`: tiempo, log, input, action map, cola de eventos, config y resolución de paths.
- `broadwayos/src/gfx`: init VI, surface y font bitmap.
- `broadwayos/src/ui`: widgets base, labels, botones, listas, textboxes y layout.
- `broadwayos/src/wm`: window manager y compositor.
- `broadwayos/src/shell`: `desktop_shell`.
- `broadwayos/src/services`: service manager, servicios base y backends IOS/null.
- `broadwayos/src/apps`: loader, apps built-in y app externa `hello`.

## Flujo de arranque
1. `main` inicializa video y servicios.
2. Resuelve storage disponible (`sd:` con fallback `usb:`).
3. Carga config (`config.ini`) para boot mode.
4. Inicializa shell o launcher.
5. Expone errores recuperables en pantalla cuando falta storage/config/manifest.
6. Loop principal: input -> eventos -> update/draw -> present.
7. Salida limpia por `HOME`/`START`.

## Layout de distribución HBC
`make package` genera:

```text
dist/apps/wiios/
  boot.dol
  meta.xml
  icon.png
  config.ini
  apps/hello/manifest.ini
```

Ese directorio se copia completo a `sd:/apps/wiios` para Wii real o se abre desde Dolphin usando `boot.dol`.
