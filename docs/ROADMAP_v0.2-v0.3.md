# Roadmap WiiOS (post v0.1.0)

## Objetivo general
Pasar de un release confiable base (`v0.1.0`) a una plataforma usable y mantenible (`v0.3.0`) manteniendo distribución por HBC.

## v0.2.0 — Robustez de plataforma

### Objetivos
- Persistencia app-level estandarizada.
- App loader menos rígido (no solo `hello`).
- Telemetría/log útil para depuración de campo.

### Entregables
1. Persistencia app-level
- Rutas:
  - `<root>/data/system/settings.ini`
  - `<root>/data/apps/<app_id>/state.ini`
- API de helper para save/load por app.

2. Catálogo de apps por manifest
- Escanear `<root>/apps/*/manifest.ini`.
- Validar `id`, `entry`, `type`.
- Mostrar estado inválido en launcher sin crash.

3. Logging persistente
- `logs/wiios.log` con rotación simple (`wiios.prev.log`).
- Log de eventos de arranque, fallback y errores de FS.

4. QA ampliada
- Checklist de regresión por escenarios negativos.
- Registro de resultados Dolphin + Wii real por build.

### Criterio de salida v0.2.0
- Apps por manifest se listan dinámicamente.
- Persistencia por app funciona al menos para 2 apps.
- Logs en disco disponibles tras reboot.

## v0.3.0 — Madurez operativa

### Objetivos
- UX más consistente y recovery completo.
- Hardening de loop principal/eventos/memoria.
- Release engineering formal.

### Entregables
1. Recovery UX
- Mensajes claros para:
  - config corrupta,
  - manifest inválido,
  - storage ausente,
  - fallback SD/USB.

2. Hardening técnico
- Métricas de cola de eventos (drops/overflow).
- Control de fallos de escritura y rollback seguro.
- Validaciones de límites en paths y buffers críticos.

3. Release engineering
- Target `release-check` en Makefile.
- Hash de artefactos (`sha256`) en release notes.
- Plantilla de publicación para GitHub Releases.

4. Documentación operativa
- Runbook de incidentes (qué revisar cuando no arranca).
- Guía de troubleshooting para usuarios.

### Criterio de salida v0.3.0
- Sin cuelgues en smoke de 10 minutos (Dolphin/Wii).
- Recovery funcional en todos los casos negativos definidos.
- Publicación reproducible con checklist y hashes.

## Fuera de alcance (hasta v0.3.0)
- Canal/WAD.
- Priiloader integrado.
- Guardado NAND tipo título oficial.

## Cadencia recomendada
- Part-time constante.
- v0.2.0: 4-5 semanas.
- v0.3.0: 4-6 semanas.
