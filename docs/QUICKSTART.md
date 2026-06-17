# Quickstart

Este flujo asume que estás en la raíz del repo y que `devkitPPC`, `libogc` y `make` están instalados.

## 1. Build
```bash
make clean && make
```

## 2. Package
```bash
make package
```

## 3. Copiar a SD
Copiar `dist/apps/wiios` a:
- `sd:/apps/wiios`

## 4. Ejecutar
- En Wii real: abrir Homebrew Channel y correr WiiOS.
- En Dolphin: abrir `dist/apps/wiios/boot.dol`.

## 5. Controles
- `A`: OK
- `B`: Back
- `Left/Right`: navegar foco/selección
- `Home` o `Start`: salir

## Limpieza
```bash
make clean
```

Esto elimina `build/` y `dist/`. Los artefactos generados no deben subirse al repo; para distribución, adjunta el package generado a una release.
