# Smoke Test Checklist - v0.1.0

## Gate técnico
1. `make clean && make`
2. `make package`
3. Verificar artefactos:
- `dist/apps/wiios/boot.dol`
- `dist/apps/wiios/meta.xml`
- `dist/apps/wiios/icon.png`
- `dist/apps/wiios/config.ini`
- `dist/apps/wiios/apps/hello/manifest.ini`

## Dolphin
1. Arranca `boot.dol`.
2. Render visible y estable.
3. Input funcional (`A/B/LEFT/RIGHT/HOME`).
4. Entrar a settings, cambiar `boot_to`, reiniciar y validar cambio.
5. Probar escenario sin manifest `hello` y verificar mensaje de estado.

## Wii real
1. Arrancar desde Homebrew Channel.
2. Validar input y navegación básica por 5 minutos.
3. Cambiar `boot_to` y confirmar persistencia tras reboot físico.
4. Confirmar salida limpia con HOME/START.

## Casos negativos
1. `config.ini` ausente: debe arrancar con defaults y mensaje explícito.
2. `manifest.ini` ausente o inválido: no debe colgarse ni pantalla negra.
3. Storage no disponible: mostrar `STORAGE ERROR` y permitir salida.
