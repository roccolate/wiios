# Roadmap WiiOS (post v0.1.x)

## Objetivo general

Pasar de una base homebrew confiable (`v0.1.x`) a una plataforma usable y
mantenible (`v0.3.0`) manteniendo distribución por Homebrew Channel.

El proyecto actual es un shell/launcher Broadway/IOS usando `devkitPPC` +
`libogc`. No es una distro Linux ni debe tratar de convertirse en una en este
roadmap inmediato.

## Baseline actual

Ya existe:

- Package reproducible para HBC.
- Render VI/XFB estable.
- Input PAD/WPAD.
- Resolver de storage con package roots y data roots.
- Persistencia atómica de `config.ini`.
- Boot modes `launcher` y `desktop`.
- Launcher con `HELLO`, `FILES`, `SETTINGS`.
- File manager básico de solo lectura.
- Settings app para alternar `boot_to`.
- Loader de manifest para la app externa fija `hello`.
- Desktop shell demo con window manager/compositor.
- Ring log en memoria.
- Pantallas/mensajes para storage/config/manifest errors.

Todavía falta:

- Catálogo dinámico de apps.
- Persistencia app-level genérica.
- Logging persistente en disco.
- Release-check formal.
- Troubleshooting/runbook.

## v0.2.0 — Robustez de plataforma

### Objetivos

- Convertir el launcher de demo fija a catálogo real.
- Estandarizar persistencia app-level.
- Agregar logging persistente útil para depuración en Wii real.
- Mantener compatibilidad con el paquete HBC actual.

### Entregables

#### 1. Catálogo dinámico de apps por manifest

- Escanear:

```text
<root>/apps/*/manifest.ini
```

- Validar campos mínimos:
  - `id`
  - `entry`
  - `type` / `kind` según el parser actual
  - nombre visible si existe
- Rechazar IDs vacíos o duplicados.
- Mostrar apps inválidas como entradas con estado de error, no como crash.
- Mantener `hello` como app demo de compatibilidad.
- Documentar el formato final de `manifest.ini`.

Criterio de salida:

- Al menos dos apps demo aparecen desde manifests distintos.
- Manifest ausente o inválido no rompe el launcher.
- El launcher ya no depende de una lista visual hardcodeada para el catálogo
  externo.

#### 2. Persistencia app-level

Rutas objetivo:

```text
<root>/data/system/settings.ini
<root>/data/apps/<app_id>/state.ini
```

API objetivo:

- helper para construir rutas seguras por `app_id`,
- `app_state_load(app_id, key, ...)`,
- `app_state_save(app_id, key, value)`,
- mkdir recursivo automático,
- escritura atómica para state.

Criterio de salida:

- Dos apps pueden guardar y cargar estado independiente.
- Un app no puede escribir fuera de su carpeta por path traversal.
- Reinicio físico conserva el estado.

#### 3. Logging persistente

Rutas objetivo:

```text
<root>/logs/wiios.log
<root>/logs/wiios.prev.log
```

Comportamiento:

- Rotación simple al arrancar.
- Ring log sigue existiendo para render/debug rápido.
- Eventos mínimos:
  - boot start/shutdown,
  - root seleccionado,
  - fallback SD/USB/FAT,
  - config missing/invalid,
  - manifest missing/invalid,
  - errores FS,
  - saves de settings/state.

Criterio de salida:

- Logs sobreviven reboot.
- Un fallo de escritura de logs no impide arrancar.
- `logs/wiios.prev.log` conserva el log anterior.

#### 4. QA ampliada

- Checklist de regresión con escenarios positivos/negativos.
- Registro manual de resultados en Dolphin y Wii real por build.
- Pruebas de config missing/invalid.
- Pruebas de manifests missing/invalid/duplicate.
- Pruebas de storage fallback.

Criterio de salida:

- Smoke test de 10 minutos sin cuelgues.
- No pantalla negra ante errores de config/manifest.
- Persistencia validada en Dolphin y Wii real.

## v0.3.0 — Madurez operativa

### Objetivos

- UX más consistente.
- Recovery completo.
- Hardening de loop principal/eventos/memoria.
- Release engineering formal.

### Entregables

#### 1. Recovery UX

Mensajes claros para:

- config corrupta,
- manifest inválido,
- app registry vacío,
- storage ausente,
- fallback SD/USB/FAT,
- fallo de escritura,
- app state corrupto.

El usuario debe poder entender qué archivo revisar sin conectar debugger.

#### 2. Hardening técnico

- Métricas de cola de eventos:
  - drops,
  - overflow,
  - eventos procesados por frame.
- Control de fallos de escritura y rollback seguro.
- Validaciones de límites en paths y buffers críticos.
- Revisión de todos los `snprintf`, buffers fijos y joins de path.
- Guardrails contra path traversal en manifests y app state.

#### 3. Release engineering

- Target `release-check` en Makefile.
- Validar que `make clean && make && make package` termina.
- Validar existencia de artefactos esperados.
- Generar hashes SHA256:

```text
boot.dol
meta.xml
icon.png
config.ini
```

- Plantilla de GitHub Release.
- Notas de release con:
  - versión,
  - commit,
  - hashes,
  - smoke-test status,
  - known limits.

#### 4. Documentación operativa

- Runbook de incidentes:
  - no arranca,
  - storage error,
  - config corrupta,
  - app no aparece,
  - app state corrupto,
  - log no se escribe.
- Guía de troubleshooting para usuarios.
- Guía de creación de apps demo.
- Formato oficial de manifest.

### Criterio de salida v0.3.0

- Sin cuelgues en smoke de 10 minutos en Dolphin y Wii real.
- Recovery funcional en todos los casos negativos definidos.
- Publicación reproducible con checklist y hashes.
- Documentación suficiente para que otra persona compile, copie a SD, ejecute y
  diagnostique fallos básicos.

## Fuera de alcance hasta v0.3.0

- Canal/WAD.
- Priiloader integrado.
- Guardado NAND tipo título oficial.
- Kernel Linux propio.
- Navegador real.
- Drivers nativos avanzados de GPU/audio/red fuera de libogc.
- Carga dinámica arbitraria sin sandbox/validación.

## Cadencia recomendada

- Part-time constante.
- v0.2.0: 4-5 semanas.
- v0.3.0: 4-6 semanas.

## Próximo paso recomendado

Crear una rama `v0.2-app-registry` y empezar por una especificación pequeña:

```text
docs/SPEC_v0.2_APP_REGISTRY.md
```

Esa spec debería congelar el formato de manifest antes de tocar el launcher.
