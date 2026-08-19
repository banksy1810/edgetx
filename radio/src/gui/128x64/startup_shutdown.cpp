/*
 * Copyright (C) EdgeTX
 *
 * Based on code named
 *   opentx - https://github.com/opentx/opentx
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "edgetx.h"
#include "hal/rgbleds.h"
#include "boards/generic_stm32/rgb_leds.h"


#define SLEEP_BITMAP_WIDTH             42
#define SLEEP_BITMAP_HEIGHT            47

const unsigned char bmp_sleep[]  = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0xf0, 
0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfc, 0xfc, 0xf8, 0xf8, 0xf0, 0xf0, 0xe0, 0xc0, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x1f, 0x1f, 0x1f, 0x1f, 0xff, 0xff, 0xff, 
0xff, 0x1f, 0x1f, 0x1f, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x0f, 0x1f, 0x3f, 0x3f, 0xfe, 0xfe, 0xff, 0xff, 0xfb, 0xfb, 0xff, 0xff, 0xfe, 0xfe, 0x3f, 0x3f, 
0x1f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x3e, 0x3e, 0x3e, 0x38, 0x70, 0xe0, 
0xe0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe0, 0xe0, 0x70, 0x38, 0x3e, 0x3e, 0x3e, 0x38, 0x10, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x18, 0x38, 0x7c, 0xf8, 0x7c, 0x1c, 0x0e, 0x0f, 0x07, 0x03, 0x03, 0x03, 0x03, 0x07, 
0x0f, 0x0e, 0x1c, 0x7c, 0xf8, 0x7c, 0x38, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#if defined(RADIO_FAMILY_T20)
constexpr uint8_t steps = NUM_FUNCTIONS_SWITCHES/2;
#elif defined(RADIO_GX12)
constexpr uint8_t steps = NUM_FUNCTIONS_SWITCHES - 2; //Exclude SA and SD
#elif defined(FUNCTION_SWITCHES)
constexpr uint8_t steps = NUM_FUNCTIONS_SWITCHES;
#endif

void drawStartupAnimation(uint32_t duration, uint32_t totalDuration)
{
  if (totalDuration == 0)
    return;

  uint8_t index = limit<uint8_t>(0, duration / (totalDuration / 5), 4);

  lcdRefreshWait();
  lcdClear();

#if defined(FUNCTION_SWITCHES)
  uint8_t index2 = limit<uint8_t>(
      0, duration / (totalDuration / (steps + 1)),
      steps);

  for (uint8_t j = 0; j < steps; j++) {
    if (index2 > j) {
#if defined(FUNCTION_SWITCHES_RGB_LEDS)
      fsLedRGB(j, 0xFFFFFF);
      rgbLedColorApply();
#else
      fsLedOn(j);
#endif
#if defined(RADIO_FAMILY_T20)
      fsLedOn(j + steps);
#endif
    }
  }
#endif

  for (uint8_t i = 0; i < 4; i++) {
    if (index > i) {
      lcdDrawFilledRect(LCD_W / 2 - 18 + 10 * i, LCD_H / 2 - 3, 6, 6, SOLID, 0);
    }
  }

  lcdRefresh();
  lcdRefreshWait();
}

void drawShutdownAnimation(uint32_t duration, uint32_t totalDuration,
                           const char* message)
{
  if (totalDuration == 0)
    return;

  uint8_t index = limit<uint8_t>(0, duration / (totalDuration / 5), 4);

  lcdRefreshWait();
  lcdClear();

#if defined(FUNCTION_SWITCHES)

  uint8_t index2 = limit<uint8_t>(
      0, duration / (totalDuration / (steps + 1)),
      steps);

  for (uint8_t j = 0; j < steps; j++) {
#if defined(FUNCTION_SWITCHES_RGB_LEDS)
    fsLedRGB(j, 0);
    if (steps - index2 > j) {
        fsLedRGB(j, 0xFFFFFF);
    }
    rgbLedColorApply();
#else
    fsLedOff(j);
#if defined(RADIO_FAMILY_T20)
    fsLedOff(j + steps);
#endif
    if (steps - index2 > j) {
      fsLedOn(j);
#if defined(RADIO_FAMILY_T20)
      fsLedOn(j + steps);
#endif
    }
#endif
  }
#endif

  for (uint8_t i = 0; i < 4; i++) {
    if (4 - index > i) {
      lcdDrawFilledRect(LCD_W / 2 - 18 + 10 * i, LCD_H / 2 - 3, 6, 6, SOLID, 0);
    }
  }
  if (message) {
    lcdDrawText((LCD_W - getTextWidth(message)) / 2, LCD_H-2*FH, message);
  }

  if (index == 0) {
    lcdClear();
for (uint16_t hold_frame = 0; hold_frame < 40; hold_frame++) {
    lcdDraw1bitBitmap((LCD_W - SLEEP_BITMAP_WIDTH) / 2, (LCD_H - SLEEP_BITMAP_HEIGHT) / 2, bmp_sleep, 0);
  
        lcdRefreshWait();
        lcdRefresh();
}
} else {
    lcdRefresh();
    lcdRefreshWait();
  }
}

void drawSleepBitmap()
{
  lcdRefreshWait();
  lcdClear();

  lcdDraw1bitBitmap((LCD_W - SLEEP_BITMAP_WIDTH) / 2,
                    (LCD_H - SLEEP_BITMAP_HEIGHT) / 2, bmp_sleep, 0);

  lcdRefresh();
  lcdRefreshWait();
}
