# WiiOS

WiiOS es un shell/launcher homebrew para Nintendo Wii, ejecutado en Broadway sobre IOS y construido con `devkitPPC` + `libogc`.

El objetivo del proyecto es ofrecer una base pequeña, mantenible y distribuible por Homebrew Channel para experimentar con una experiencia tipo sistema operativo sobre Wii.

## Estado actual (`v0.1.0`)
- Arranque por HBC con package reproducible.
- Render estable en VI/XFB.
- Input real PAD/WPAD.
- Resolución de storage con prioridad SD y fallback USB para config.
- Persistencia de `boot_to` en `config.ini` con escritura atómica.
- Manejo explícito de estados de arranque y errores de manifest.

## Estructura del repo
- `broadwayos/`: runtime Wii, core, video, input, servicios, apps y shell.
- `shared/`: tipos compartidos, eventos, parser INI y parser de manifests.
- `assets/`: metadata e icono HBC, fuentes e iconos internos.
- `sdroot_template/`: archivos que se copian al paquete HBC.
- `docs/`: arquitectura, quickstart, smoke tests, límites y roadmap.
- `RELEASE_NOTES_*.md`: notas de versiones publicadas.

## Alcance actual
- Distribución solo por Homebrew Channel.
- Sin canal/WAD.
- Sin Priiloader integrado.
- Sin acceso NAND tipo título oficial.

## Requisitos
- `devkitPPC`
- `libogc`
- `make`

Por defecto el `Makefile` espera:

```bash
DEVKITPRO=/opt/devkitpro
DEVKITPPC=/opt/devkitpro/devkitPPC
```

## Build
```bash
make clean && make
```

## Package (HBC)
```bash
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

## Documentación
- [Quickstart](docs/QUICKSTART.md)
- [Arquitectura](docs/ARCH.md)
- [Smoke test v0.1.0](docs/SMOKE_TEST_v0.1.0.md)
- [Límites conocidos](docs/KNOWN_LIMITS.md)
- [Roadmap v0.2-v0.3](docs/ROADMAP_v0.2-v0.3.md)

## Artefactos
Los directorios `build/` y `dist/`, junto con binarios empaquetados (`*.dol`, `*.elf`, `*.rar`, `*.zip`, `*.7z`), no se versionan. Para publicar builds reproducibles, genera el paquete con `make package` y adjúntalo a GitHub Releases.
