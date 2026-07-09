# Quickstart

Este flujo asume que estás en la raíz del repo y que `devkitPPC`, `libogc` y
`make` están instalados.

WiiOS actualmente es un shell/launcher homebrew para Wii sobre Broadway/IOS. No
es una distro Linux ni un reemplazo del menú del sistema.

## 1. Requisitos

Variables por defecto:

```bash
DEVKITPRO=/opt/devkitpro
DEVKITPPC=/opt/devkitpro/devkitPPC
```

Puedes exportarlas si tu instalación de devkitPro vive en otra ruta:

```bash
export DEVKITPRO=/opt/devkitpro
export DEVKITPPC=/opt/devkitpro/devkitPPC
```

## 2. Build

```bash
make clean && make
```

Salida esperada:

```text
build/wiios.elf
build/wiios.dol
```

## 3. Package

```bash
make package
```

Salida esperada:

```text
dist/apps/wiios/boot.dol
dist/apps/wiios/meta.xml
dist/apps/wiios/icon.png
dist/apps/wiios/config.ini
dist/apps/wiios/apps/hello/manifest.ini
```

## 4. Copiar a SD

Copia el directorio completo:

```text
dist/apps/wiios/
```

a:

```text
sd:/apps/wiios/
```

El runtime también intenta resolver roots equivalentes como `sd0:/apps/wiios`,
`usb:/apps/wiios`, `fat:/apps/wiios`, `sd:/wiios` o `usb:/wiios` cuando el
backend los expone, pero la ruta recomendada para Wii real sigue siendo:

```text
sd:/apps/wiios/
```

## 5. Ejecutar

### Wii real

1. Inserta la SD.
2. Abre Homebrew Channel.
3. Ejecuta WiiOS.

### Dolphin

1. Abre:

```text
dist/apps/wiios/boot.dol
```

2. Valida render, input y persistencia de config.

## 6. Controles

| Input | Acción |
|---|---|
| `A` | OK / abrir / alternar setting |
| `B` | Back. En modo desktop guarda `boot_to=launcher` |
| `LEFT` / `RIGHT` | Navegar foco/selección |
| `HOME` o `START` | Salir |

## 7. Launcher actual

El launcher actual tiene tres tiles:

```text
HELLO   FILES   SETTINGS
```

- `HELLO`: usa la app externa demo si existe un manifest válido en
  `apps/hello/manifest.ini`.
- `FILES`: lista el directorio activo `<root>/apps` en modo solo lectura.
- `SETTINGS`: alterna `boot_to=launcher` / `boot_to=desktop` y guarda
  `config.ini`.

El catálogo dinámico de apps todavía no está implementado. Solo `hello` está
conectado al loader externo.

## 8. Configuración de arranque

Archivo:

```text
<root>/config.ini
```

Ejemplo launcher:

```ini
[boot]
boot_to=launcher
```

Ejemplo desktop:

```ini
[boot]
boot_to=desktop
```

Si `config.ini` falta, WiiOS arranca con defaults y muestra un mensaje de estado.

## 9. Limpieza

```bash
make clean
```

Esto elimina `build/` y `dist/`.

Los artefactos generados no deben subirse al repo. Para distribución, adjunta el
package generado a una GitHub Release.
