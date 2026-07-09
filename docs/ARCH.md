# Arquitectura actual (v0.1.x)

Este documento describe el estado actual del árbol de código. WiiOS es un
runtime/shell homebrew para Nintendo Wii sobre Broadway/IOS usando `libogc`; no
es todavía una distro Linux ni un reemplazo del menú del sistema.

## Estado actual

- Runtime Broadway sobre IOS usando `libogc`.
- Video por VI/XFB con `WiiSurface` como superficie de dibujo.
- Input PAD/WPAD convertido a acciones de alto nivel.
- Servicios centralizados por `service_manager` con backend IOS/null.
- FS sobre FAT usando `fatInitDefault()` y operaciones `fopen`, `opendir`,
  `rename`, `mkdir`, `remove`.
- Config y manifests en formato INI.
- Resolución de storage con prioridad a package roots (`*/apps/wiios`) y
  fallback a roots de datos (`*/wiios`).
- Persistencia de `config.ini` con escritura a `config.ini.tmp` y `rename`.
- Estados explícitos para arranque, fallback, errores de storage, config y
  manifest.
- Ring log en memoria con dump a stdout al cerrar.

## Módulos principales

- `shared/`: tipos, eventos, servicios, parser INI, parser de manifest y ring
  log.
- `broadwayos/src/core`: tiempo, log, input, action map, cola de eventos,
  config de arranque y resolución de paths.
- `broadwayos/src/gfx`: inicialización VI, superficie y fuente bitmap.
- `broadwayos/src/ui`: widgets base, labels, botones, listas, textboxes y
  layout.
- `broadwayos/src/wm`: window manager y compositor.
- `broadwayos/src/shell`: `desktop_shell` demo.
- `broadwayos/src/services`: service manager, servicios base y backends IOS/null.
- `broadwayos/src/apps`: loader, apps built-in y app externa `hello`.
- `assets/`: metadatos HBC, icono y assets internos.
- `sdroot_template/`: contenido copiado al paquete HBC por `make package`.

## Resolución de storage

El resolver intenta encontrar un root activo en este orden conceptual:

1. Package roots con `config.ini` existente:
   - `sd:/apps/wiios`
   - `sd0:/apps/wiios`
   - `sd1:/apps/wiios`
   - `usb:/apps/wiios`
   - `usb0:/apps/wiios`
   - `usb1:/apps/wiios`
   - `fat:/apps/wiios`
   - `fat0:/apps/wiios`
   - `fat1:/apps/wiios`
2. Data roots con `config.ini` existente:
   - `sd:/wiios`
   - `sd0:/wiios`
   - `sd1:/wiios`
   - `usb:/wiios`
   - `usb0:/wiios`
   - `usb1:/wiios`
   - `fat:/wiios`
   - `fat0:/wiios`
   - `fat1:/wiios`
3. Package roots existentes sin config.
4. Data roots existentes sin config.
5. Device roots disponibles.
6. Fallback final a `sd:/apps/wiios`.

Si no existe `config.ini`, el runtime marca `config_missing` y muestra estado de
arranque con defaults.

## Configuración de arranque

La configuración principal vive en el root activo:

```ini
[boot]
boot_to=launcher

[system]
language=es
theme=default
schema=1
```

Valores aceptados para `boot_to`:

- `launcher`
- `desktop`

La app Settings alterna este valor y lo guarda de forma atómica.

## Modelo de apps actual

El modelo de apps todavía no es un registry dinámico. Actualmente hay una mezcla
entre apps built-in y una app externa fija:

- `launcher_app`: launcher principal con tiles `HELLO`, `FILES`, `SETTINGS`.
- `filemgr_app`: lista el directorio `<root>/apps` en modo solo lectura.
- `settings_app`: alterna `boot_to=launcher` / `boot_to=desktop`.
- `hello_app`: app externa demo cargada únicamente desde
  `<root>/apps/hello/manifest.ini`.
- `desktop_shell`: modo desktop demo con dos ventanas: `Log Viewer` y
  `System Status`.
- `logviewer_app`: fuente presente, pero el log persistente en disco todavía no
  está implementado ni conectado como flujo principal.

El loader lee el manifest, valida campos básicos y acepta únicamente `id=hello`.
El futuro `v0.2.0` debe reemplazar esto por un catálogo dinámico de
`apps/*/manifest.ini`.

## Flujo de arranque

1. `main` inicializa video VI/XFB.
2. Inicializa `service_manager`; si falla, muestra pantalla `STORAGE ERROR` con
   retry por `A`, salida por `HOME` y auto-exit.
3. Inicializa el resolver de storage.
4. Intenta cargar el manifest externo fijo de `hello` con fallback de roots.
5. Carga `config.ini` para decidir `launcher` o `desktop`.
6. Inicializa launcher o desktop shell.
7. Inicializa input.
8. Loop principal:
   - input,
   - cola de eventos,
   - update,
   - draw,
   - overlay de estado,
   - `vi_present`.
9. Salida limpia con `HOME`/`START`.
10. Dump del ring log a stdout y apagado de video/servicios.

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

Ese directorio se copia completo a `sd:/apps/wiios` para Wii real o se abre desde
Dolphin usando `boot.dol`.

## Límites arquitectónicos actuales

- No hay carga dinámica de binarios externos arbitrarios.
- No hay registry dinámico de manifests.
- No hay persistencia app-level genérica.
- El log es en memoria; no hay `logs/wiios.log` todavía.
- No hay canal/WAD, Priiloader ni integración NAND estilo título oficial.
- El desktop shell es demo; no es un desktop de producción.
