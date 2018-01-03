/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * Neopixel i2c slave support
 */

#ifndef __NEOPIXEL_SLAVE_H__
#define __NEOPIXEL_SLAVE_H__

#include "../../inc/MarlinConfig.h"

#include <Wire.h>

#define NEOPIXEL_STATE_NULL 0
#define NEOPIXEL_STATE_BED_HEATUP 1
#define NEOPIXEL_STATE_HOTEND_HEATUP 2
#define NEOPIXEL_STATE_PRINTING 3
#define NEOPIXEL_STATE_PRINTING_FINISHED 4
#define NEOPIXEL_STATE_PRINTING_PAUSED 5
#define NEOPIXEL_STATE_PRINTING_STOPPED 6
#define NEOPIXEL_STATE_CUSTOM_COLOR 7
#define NEOPIXEL_STATE_CUSTOM_PIXEL_COLOR 8
#define NEOPIXEL_STATE_IDLE 9

void setup_neopixel_slave(uint16_t maxTempBed, uint16_t maxTempHotend, uint8_t stripNumber=0);
void set_neopixel_color(const uint32_t color, uint8_t stripNumber=0);
void set_neopixel_color(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t white, uint8_t stripNumber=0);
void set_neopixel_pixel(const uint32_t color, uint8_t pixel, uint8_t stripNumber=0);
void set_neopixel_pixel(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t white, uint8_t pixel, uint8_t stripNumber=0);
void set_neopixel_hotend(uint16_t currentTemp, uint16_t targetTemp, uint16_t maxTemp, uint8_t stripNumber=0);
void set_neopixel_heatbed(uint16_t currentTemp, uint16_t targetTemp, uint16_t maxTemp, uint8_t stripNumber=0);
void set_neopixel_progress(uint8_t progress, uint8_t state, uint8_t stripNumber=0); 
void set_neopixel_state(uint8_t state, uint8_t stripNumber=0); 
void send_neopixel_data();

#endif // __NEOPIXEL_SLAVE_H__
