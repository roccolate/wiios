#ifndef WIIOS_FONT_BITMAP_H
#define WIIOS_FONT_BITMAP_H

#include "surface.h"

/*
 * Bitmap font: A-Z, 0-9, space, '-', '.', ':', '/'.
 * Lowercase letters are uppercased before lookup.
 * Any other character renders the '?' fallback glyph.
 */

void font_draw_text(WiiSurface *s, wii_i32 x, wii_i32 y, const char *text, wii_u32 color, wii_i32 scale);
wii_i32 font_text_width(const char *text, wii_i32 scale);

#endif
