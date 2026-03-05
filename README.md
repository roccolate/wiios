# WiiOS

WiiOS es un shell/launcher homebrew para Nintendo Wii (Broadway sobre IOS), construido con `devkitPPC` y `libogc`.

## Estado actual (`v0.1.0`)
- Arranque por HBC con package reproducible.
- Render estable en VI/XFB.
- Input real PAD/WPAD.
- Resolución de storage con prioridad SD y fallback USB para config.
- Persistencia de `boot_to` en `config.ini` con escritura atómica.
- Manejo explícito de estados de arranque y errores de manifest.

## Alcance actual
- Distribución solo por Homebrew Channel.
- Sin canal/WAD.
- Sin Priiloader integrado.

## Requisitos
- `devkitPPC`
- `libogc`
- `make`

## Build
```bash
cd wiios
make clean && make
```

## Package (HBC)
```bash
cd wiios
make package
```

Salida esperada:
- `dist/apps/wiios/boot.dol`
- `dist/apps/wiios/meta.xml`
- `dist/apps/wiios/icon.png`
- `dist/apps/wiios/config.ini`
- `dist/apps/wiios/apps/hello/manifest.ini`

## Probar en Dolphin
1. Abrir `dist/apps/wiios/boot.dol`.
2. Validar render.
3. Validar controles.
4. Cambiar `boot_to` y confirmar persistencia tras reinicio.

## Probar en Wii real
1. Copiar `dist/apps/wiios` a `sd:/apps/wiios`.
2. Ejecutar desde Homebrew Channel.
3. Validar render/input y persistencia de `boot_to`.

## Controles
- `A`: aceptar
- `B`: atrás
- `LEFT/RIGHT`: foco o selección
- `HOME` (Wiimote) / `START` (GC): salir
