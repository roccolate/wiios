# Arquitectura MVP

- `shared/`: tipos, eventos, servicios ABI.
- `broadwayos/`: runtime principal en Broadway.
- `assets/`: recursos base (icono y fuente placeholder).
- `sdroot_template/`: layout inicial para apps externas.

MS1 implementa:
- Cola de eventos
- Logger en ring buffer
- Surface software simple
- UI widgets base
- App builtin `logviewer_app`
