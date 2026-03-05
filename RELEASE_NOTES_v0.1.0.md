# Release Notes - v0.1.0

## Objetivo
Entregar una base confiable sobre `v0.0.1` con foco en storage robusto, persistencia de configuración y manejo explícito de errores.

## Highlights
- Resolución de rutas con prioridad SD y fallback USB.
- API FS ampliada para lectura/escritura/renombrado/directorios.
- Persistencia de `boot_to` con escritura atómica.
- Estados de arranque y errores visibles en UI.

## Requisitos
- devkitPPC
- libogc
- make

## Artefactos
- `dist/apps/wiios/boot.dol`
- `dist/apps/wiios/meta.xml`
- `dist/apps/wiios/icon.png`
- `dist/apps/wiios/config.ini`
- `dist/apps/wiios/apps/hello/manifest.ini`

## Instalación (HBC)
1. `make package`
2. Copiar `dist/apps/wiios` a `sd:/apps/wiios`
3. Ejecutar desde Homebrew Channel

## Riesgos conocidos
- App loader aún limitado para catálogo de apps.
- Persistencia app-level general en progreso.

## Rollback
- Reemplazar `sd:/apps/wiios` por el package de `v0.0.1`.
