#ifndef EMULOGO_H
#define EMULOGO_H

#include <exec/types.h>

#define EMULOGO_WIDTH  200
#define EMULOGO_HEIGHT 200

/* 1bpp mask, MSB-first, row-major: EMULOGO_WIDTH/8 bytes per row */
extern const UBYTE EmuLogoMask[(EMULOGO_WIDTH / 8) * EMULOGO_HEIGHT];

#endif /* EMULOGO_H */
