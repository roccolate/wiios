DEVKITPRO ?= /opt/devkitpro
DEVKITPPC ?= /opt/devkitpro/devkitPPC

ifeq ($(strip $(DEVKITPRO)),)
$(error DEVKITPRO not set)
endif
ifeq ($(strip $(DEVKITPPC)),)
$(error DEVKITPPC not set)
endif

TARGET := wiios
BUILD := build
DIST := dist/apps/wiios
CC := $(DEVKITPPC)/bin/powerpc-eabi-gcc
ELF2DOL := $(DEVKITPRO)/tools/bin/elf2dol

CFLAGS := -std=c11 -Wall -Wextra -Werror -O2 -DGEKKO -mrvl -mcpu=750 -meabi -mhard-float \
	-Ishared/include -Ibroadwayos/include -I$(DEVKITPRO)/libogc/include
LDFLAGS := -specs=$(DEVKITPRO)/libogc/share/rvl.specs -L$(DEVKITPRO)/libogc/lib/wii \
	-lwiiuse -lbte -lfat -logc -lm

SRC := \
  shared/src/ringlog.c \
  shared/src/ini_parser.c \
  shared/src/manifest_ini.c \
  broadwayos/src/main.c \
  broadwayos/src/core/time.c \
  broadwayos/src/core/log.c \
  broadwayos/src/core/input.c \
  broadwayos/src/core/action_map.c \
  broadwayos/src/core/event_queue.c \
  broadwayos/src/gfx/vi.c \
  broadwayos/src/gfx/surface.c \
  broadwayos/src/gfx/font_bitmap.c \
  broadwayos/src/ui/widget.c \
  broadwayos/src/ui/label.c \
  broadwayos/src/ui/button.c \
  broadwayos/src/ui/listview.c \
  broadwayos/src/ui/textbox.c \
  broadwayos/src/ui/layout.c \
  broadwayos/src/wm/window_manager.c \
  broadwayos/src/wm/compositor.c \
  broadwayos/src/shell/desktop_shell.c \
  broadwayos/src/services/service_manager.c \
  broadwayos/src/services/svc_log.c \
  broadwayos/src/services/svc_time.c \
  broadwayos/src/services/svc_input.c \
  broadwayos/src/services/svc_fs.c \
  broadwayos/src/services/backends/backend_ios.c \
  broadwayos/src/services/backends/backend_null.c \
  broadwayos/src/mem/heap.c \
  broadwayos/src/mem/arena.c \
  broadwayos/src/apps/loader/app_loader.c \
  broadwayos/src/apps/builtin/launcher_app/launcher_app.c \
  broadwayos/src/apps/builtin/filemgr_app/filemgr_app.c \
  broadwayos/src/apps/builtin/settings_app/settings_app.c \
  broadwayos/src/apps/external/hello/hello_app.c \
  broadwayos/src/apps/builtin/logviewer_app/logviewer_app.c

OUT_ELF := $(BUILD)/$(TARGET).elf
OUT_DOL := $(BUILD)/$(TARGET).dol

.PHONY: all clean package

all: $(OUT_DOL)

$(OUT_ELF): $(SRC)
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OUT_DOL): $(OUT_ELF)
	$(ELF2DOL) $< $@

package: $(OUT_DOL)
	rm -rf $(DIST)
	mkdir -p $(DIST)
	cp $(OUT_DOL) $(DIST)/boot.dol
	cp assets/hbc/meta.xml $(DIST)/meta.xml
	cp assets/hbc/icon.png $(DIST)/icon.png
	cp -r sdroot_template/wiios/* $(DIST)/

clean:
	rm -rf $(BUILD) dist
