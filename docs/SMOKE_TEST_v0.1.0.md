# Smoke Test Checklist - v0.1.x

Checklist manual para validar el estado actual del runtime. Ejecutar en Dolphin y,
cuando sea posible, en Wii real.

## Gate técnico

1. `make clean && make`
2. `make package`
3. Verificar artefactos:
   - `dist/apps/wiios/boot.dol`
   - `dist/apps/wiios/meta.xml`
   - `dist/apps/wiios/icon.png`
   - `dist/apps/wiios/config.ini`
   - `dist/apps/wiios/apps/hello/manifest.ini`
4. Confirmar que `build/` y `dist/` no se suben al repo.

## Dolphin

1. Abrir `dist/apps/wiios/boot.dol`.
2. Confirmar render visible y estable.
3. Confirmar input básico:
   - `A`
   - `B`
   - `LEFT`
   - `RIGHT`
   - `HOME` / `START`
4. En launcher, navegar entre `HELLO`, `FILES`, `SETTINGS`.
5. Abrir `HELLO` y confirmar que la demo aparece si el manifest es válido.
6. Abrir `FILES` y confirmar que lista el root activo de `apps/` o muestra estado
   de error claro.
7. Abrir `SETTINGS`, presionar `A`, confirmar cambio de `BOOT TO LAUNCHER`.
8. Reiniciar y confirmar que `boot_to` persistió en `config.ini`.
9. Cambiar a desktop mode desde Settings y reiniciar.
10. En desktop mode, confirmar:
    - barra superior `WIIOS DESKTOP`,
    - ventanas demo visibles,
    - `LEFT`/`RIGHT` cambian foco,
    - `A` activa la ventana de sistema,
    - `B` guarda `boot_to=launcher`.
11. Reiniciar y confirmar regreso a launcher.

## Wii real

1. Copiar `dist/apps/wiios/` a `sd:/apps/wiios/`.
2. Arrancar desde Homebrew Channel.
3. Validar render y navegación básica por 5 minutos.
4. Validar `HELLO`, `FILES`, `SETTINGS`.
5. Cambiar `boot_to` y confirmar persistencia tras reboot físico.
6. Confirmar salida limpia con `HOME`/`START`.
7. Si hay USB disponible, repetir una prueba copiando el paquete a un root USB
   compatible expuesto por libogc.

## Casos negativos

Usar una copia temporal del package para no romper el build base.

### Config ausente

1. Eliminar `config.ini`.
2. Arrancar.
3. Resultado esperado:
   - arranca con defaults,
   - muestra `CONFIG MISSING - USING DEFAULTS`,
   - Settings puede guardar un nuevo `config.ini`.

### Config inválida

1. Corromper `[boot] boot_to` con un valor inválido.
2. Arrancar.
3. Resultado esperado:
   - no se cuelga,
   - muestra `CONFIG INVALID - USING DEFAULTS`,
   - Settings puede guardar un valor válido.

### Manifest `hello` ausente

1. Eliminar `apps/hello/manifest.ini`.
2. Arrancar.
3. Resultado esperado:
   - no hay pantalla negra,
   - muestra `APP MANIFEST MISSING`,
   - el launcher sigue respondiendo.

### Manifest `hello` inválido

1. Corromper `apps/hello/manifest.ini`.
2. Arrancar.
3. Resultado esperado:
   - no se cuelga,
   - muestra `APP MANIFEST INVALID`,
   - `FILES` y `SETTINGS` siguen funcionando.

### Storage no disponible

1. Arrancar sin storage disponible o forzar fallo de backend en entorno de test.
2. Resultado esperado:
   - pantalla `STORAGE ERROR`,
   - `A` reintenta,
   - `HOME` sale,
   - auto-exit tras timeout si no hay input.

## Resultado mínimo aceptable

- No cuelgues durante 10 minutos de navegación en Dolphin.
- No pantalla negra ante config/manifest faltante.
- `boot_to` persiste después de reiniciar.
- Salida limpia con `HOME`/`START`.
