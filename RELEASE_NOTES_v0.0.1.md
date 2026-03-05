# Release Notes - v0.0.1

## Objetivo
Congelar el primer demo funcional y auditable de WiiOS como prueba de concepto.

## Requisitos
- devkitPPC
- libogc
- make

## Artefactos entregados
- `dist/apps/wiios/boot.dol`
- `dist/apps/wiios/meta.xml`
- `dist/apps/wiios/icon.png`
- `dist/apps/wiios/config.ini`
- `dist/apps/wiios/apps/hello/manifest.ini`

## Instalación (HBC)
1. Ejecutar `make package`.
2. Copiar `dist/apps/wiios` a `sd:/apps/wiios`.
3. Ejecutar desde Homebrew Channel.

## Riesgos conocidos
- Persistencia de settings aún en maduración.
- File manager no es explorador completo.
- No se soporta instalación como canal/WAD.

## Rollback simple
- Eliminar `sd:/apps/wiios`.
- Restaurar la versión anterior del homebrew en `sd:/apps`.
