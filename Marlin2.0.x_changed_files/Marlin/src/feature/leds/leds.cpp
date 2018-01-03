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
 * leds.cpp - Marlin RGB LED general support
 */

#include "../../inc/MarlinConfig.h"

#if HAS_COLOR_LEDS

#include "leds.h"

#if ENABLED(BLINKM)
  #include "blinkm.h"
#endif

#if ENABLED(PCA9632)
  #include "pca9632.h"
#endif

#if ENABLED(NEOPIXEL_SLAVE_LED)
  #include "neopixel_slave.h"
#endif

#if ENABLED(LED_COLOR_PRESETS)
  const LEDColor LEDLights::defaultLEDColor = MakeLEDColor(
    LED_USER_PRESET_RED,
    LED_USER_PRESET_GREEN,
    LED_USER_PRESET_BLUE,
    LED_USER_PRESET_WHITE,
    LED_USER_PRESET_BRIGHTNESS
  );
#endif

#if ENABLED(LED_CONTROL_MENU)
  LEDColor LEDLights::color;
  bool LEDLights::lights_on;
#endif

LEDLights leds;

void LEDLights::setup() {
  #if ENABLED(NEOPIXEL_LED)
    setup_neopixel();
  #endif
  #if ENABLED(NEOPIXEL_SLAVE_LED)
    setup_neopixel_slave(HEATER_0_MAXTEMP, BED_MAXTEMP,0);
    LEDLights::set_state(NEOPIXEL_STATE_IDLE); 
  #endif
  #if ENABLED(LED_USER_PRESET_STARTUP)
    set_default();
  #endif
}

void LEDLights::set_color(const LEDColor &incol
  #if (ENABLED(NEOPIXEL_LED) || ENABLED(NEOPIXEL_SLAVE_LED))
    , bool isSequence/*=false*/
  #endif
  #if ENABLED(NEOPIXEL_SLAVE_LED)
    , uint8_t pixel/*=0*/
  #endif  
) {

  #if ENABLED(NEOPIXEL_LED)

    const uint32_t neocolor = pixels.Color(incol.r, incol.g, incol.b, incol.w);
    static uint16_t nextLed = 0;

    pixels.setBrightness(incol.i);
    if (!isSequence)
      set_neopixel_color(neocolor);
    else {
      pixels.setPixelColor(nextLed, neocolor);
      pixels.show();
      if (++nextLed >= pixels.numPixels()) nextLed = 0;
      return;
    }

  #endif
  
  #if ENABLED(NEOPIXEL_SLAVE_LED)

    static uint8_t nextLed = 0;
    if (pixel==0) {
      if (!isSequence)
        set_neopixel_color(incol.r, incol.g, incol.b, 
          #if HAS_WHITE_LED
            incol.w
          #else
            0
          #endif
          , 0);
      else {
        set_neopixel_pixel(incol.r, incol.g, incol.b, 
          #if HAS_WHITE_LED
            incol.w
          #else
            0
          #endif
          , nextLed, 0);
        if (++nextLed >= NEOPIXEL_PIXELS) nextLed = 0;
        return;
      }
    }
    else {
      set_neopixel_pixel(incol.r, incol.g, incol.b, 
        #if HAS_WHITE_LED
          incol.w
        #else
          0
        #endif
        , pixel-1, 0);
    }
  #endif

  #if ENABLED(BLINKM)

    // This variant uses i2c to send the RGB components to the device.
    blinkm_set_led_color(incol);

  #endif

  #if ENABLED(RGB_LED) || ENABLED(RGBW_LED)

    // This variant uses 3-4 separate pins for the RGB(W) components.
    // If the pins can do PWM then their intensity will be set.
    WRITE(RGB_LED_R_PIN, incol.r ? HIGH : LOW);
    WRITE(RGB_LED_G_PIN, incol.g ? HIGH : LOW);
    WRITE(RGB_LED_B_PIN, incol.b ? HIGH : LOW);
    analogWrite(RGB_LED_R_PIN, incol.r);
    analogWrite(RGB_LED_G_PIN, incol.g);
    analogWrite(RGB_LED_B_PIN, incol.b);

    #if ENABLED(RGBW_LED)
      WRITE(RGB_LED_W_PIN, incol.w ? HIGH : LOW);
      analogWrite(RGB_LED_W_PIN, incol.w);
    #endif

  #endif

  #if ENABLED(PCA9632)
    // Update I2C LED driver
    pca9632_set_led_color(incol);
  #endif

  #if ENABLED(LED_CONTROL_MENU)
    // Don't update the color when OFF
    lights_on = !incol.is_off();
    if (lights_on) color = incol;
  #endif
}
#if ENABLED(NEOPIXEL_SLAVE_LED)
  void LEDLights::set_pixel(const LEDColor &incol, uint8_t p){
    set_neopixel_pixel(incol.r, incol.g, incol.b, 
      #if HAS_WHITE_LED
        incol.w
      #else
        0
      #endif
      , p, 0);
  }
#endif

#if ENABLED(NEOPIXEL_SLAVE_LED)
  void LEDLights::set_state(uint8_t state, uint16_t currentTemp, uint16_t targetTemp, uint16_t maxTemp){
    switch (state){
      case NEOPIXEL_STATE_NULL: //No state from printer
        set_neopixel_state(NEOPIXEL_STATE_NULL,0);
        break;
      case NEOPIXEL_STATE_BED_HEATUP: //Bed heatup
        set_neopixel_heatbed(currentTemp, targetTemp, maxTemp, 0);
        break;
      case NEOPIXEL_STATE_HOTEND_HEATUP: //Hotend heatup
        set_neopixel_hotend(currentTemp, targetTemp, maxTemp, 0);
        break;
      case NEOPIXEL_STATE_PRINTING: //Printing
        set_neopixel_state(NEOPIXEL_STATE_PRINTING,0);
        break;
      case NEOPIXEL_STATE_PRINTING_FINISHED: //Printing finished
        set_neopixel_state(NEOPIXEL_STATE_PRINTING_FINISHED,0);
        break;
      case NEOPIXEL_STATE_PRINTING_PAUSED: //Printing paused
        set_neopixel_state(NEOPIXEL_STATE_PRINTING_PAUSED,0);
        break;
      case NEOPIXEL_STATE_PRINTING_STOPPED: //Printing stopped
        set_neopixel_state(NEOPIXEL_STATE_PRINTING_STOPPED,0);
        break;
      case NEOPIXEL_STATE_CUSTOM_COLOR: //Show custom strip color
        set_neopixel_state(NEOPIXEL_STATE_CUSTOM_COLOR,0);
        break;
      case NEOPIXEL_STATE_CUSTOM_PIXEL_COLOR: //Show custom pixel color
        set_neopixel_state(NEOPIXEL_STATE_CUSTOM_PIXEL_COLOR,0);
        break;
      case NEOPIXEL_STATE_IDLE: //Idle
        set_neopixel_state(NEOPIXEL_STATE_IDLE,0);
        break;
      default:
        break;
    };
  };
#endif
  

void LEDLights::set_white() {
  #if ENABLED(RGB_LED) || ENABLED(RGBW_LED) || ENABLED(BLINKM) || ENABLED(PCA9632)
    set_color(LEDColorWhite());
  #endif
  #if ENABLED(NEOPIXEL_LED)
    set_neopixel_color(pixels.Color(NEO_WHITE));
  #endif
}

#if ENABLED(LED_CONTROL_MENU)
  void LEDLights::toggle() { if (lights_on) set_off(); else update(); }
#endif

#endif // HAS_COLOR_LEDS
