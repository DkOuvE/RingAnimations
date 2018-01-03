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
 * Marlin RGB LED general support
 */

#include "../../inc/MarlinConfig.h"

#if ENABLED(NEOPIXEL_SLAVE_LED)
  #include "leds.h"
  #include "neopixel_slave.h"
  
  
  uint8_t neopixel_i2c_buffer[21];
  uint8_t neopixel_i2c_buffer_length=0;
  
void setup_neopixel_slave(uint16_t maxTempBed, uint16_t maxTempHotend, uint8_t stripNumber){ 
  leds.progress_lock = true;
  neopixel_i2c_buffer[0]='i';
  neopixel_i2c_buffer[1]=0;
  neopixel_i2c_buffer[2]=highByte(maxTempBed);
  neopixel_i2c_buffer[3]=lowByte(maxTempBed);
  neopixel_i2c_buffer[4]=highByte(maxTempHotend);
  neopixel_i2c_buffer[5]=lowByte(maxTempHotend);
  neopixel_i2c_buffer_length =6;
  send_neopixel_data();
  safe_delay(1000);
  
  #if ENABLED(NEOPIXEL_STARTUP_TEST)
    safe_delay(1000);
    set_neopixel_color(255,0,0,0,0);  // red
    safe_delay(1000);
    set_neopixel_color(0,255,0,0,0);  // green
    safe_delay(1000);
    set_neopixel_color(0,0,255,0,0);  // blue
    safe_delay(1000);
  #endif

  #if ENABLED(LED_USER_PRESET_STARTUP)
    set_neopixel_color(LED_USER_PRESET_RED, LED_USER_PRESET_GREEN, LED_USER_PRESET_BLUE);
  #else
    set_neopixel_color(0,0,0,0,0);
  #endif
}

void set_neopixel_color(const uint32_t color, uint8_t stripNumber) {
  neopixel_i2c_buffer[0]='c';
  neopixel_i2c_buffer[1]=NEOPIXEL_STATE_CUSTOM_COLOR;
  neopixel_i2c_buffer[2]=((color>>24)&0xFF);
  neopixel_i2c_buffer[3]=((color>>16)&0xFF);
  neopixel_i2c_buffer[4]=((color>>8)&0xFF);
  neopixel_i2c_buffer[5]=(color&0xFF);
  neopixel_i2c_buffer[6]=0;
  neopixel_i2c_buffer[7]=stripNumber;  
  neopixel_i2c_buffer_length =8;
  send_neopixel_data();
}

void set_neopixel_color(const uint8_t red,const uint8_t green, const uint8_t blue, const uint8_t white, uint8_t stripNumber) {
  neopixel_i2c_buffer[0]='c';
  neopixel_i2c_buffer[1]=NEOPIXEL_STATE_CUSTOM_COLOR;
  neopixel_i2c_buffer[2]=white;  
  neopixel_i2c_buffer[3]=red;
  neopixel_i2c_buffer[4]=green;
  neopixel_i2c_buffer[5]=blue;
  neopixel_i2c_buffer[6]=0;
  neopixel_i2c_buffer[7]=stripNumber; 
  neopixel_i2c_buffer_length =8;
  send_neopixel_data();
}

void set_neopixel_pixel(const uint32_t color, uint8_t pixel, uint8_t stripNumber) {
  neopixel_i2c_buffer[0]='c';
  neopixel_i2c_buffer[1]=NEOPIXEL_STATE_CUSTOM_PIXEL_COLOR;
  neopixel_i2c_buffer[2]=((color>>24)&0xFF);
  neopixel_i2c_buffer[3]=((color>>16)&0xFF);
  neopixel_i2c_buffer[4]=((color>>8)&0xFF);
  neopixel_i2c_buffer[5]=(color&0xFF);
  neopixel_i2c_buffer[6]=pixel;
  neopixel_i2c_buffer[7]=stripNumber;
  neopixel_i2c_buffer_length =8;
  send_neopixel_data();
}

void set_neopixel_pixel(const uint8_t red,const uint8_t green, const uint8_t blue, const uint8_t white, uint8_t pixel, uint8_t stripNumber) {
  neopixel_i2c_buffer[0]='c';
  neopixel_i2c_buffer[1]=NEOPIXEL_STATE_CUSTOM_PIXEL_COLOR;
  neopixel_i2c_buffer[2]=white;
  neopixel_i2c_buffer[3]=red;
  neopixel_i2c_buffer[4]=green;
  neopixel_i2c_buffer[5]=blue;
  neopixel_i2c_buffer[6]=pixel;
  neopixel_i2c_buffer[7]=stripNumber;
  neopixel_i2c_buffer_length =8;
  send_neopixel_data();
}

void set_neopixel_hotend(uint16_t currentTemp,uint16_t targetTemp, uint16_t maxTemp, uint8_t stripNumber){
  neopixel_i2c_buffer[0]='h';
  neopixel_i2c_buffer[1]=NEOPIXEL_STATE_HOTEND_HEATUP;
  neopixel_i2c_buffer[2]=highByte(currentTemp);
  neopixel_i2c_buffer[3]=lowByte(currentTemp);
  neopixel_i2c_buffer[4]=highByte(targetTemp);
  neopixel_i2c_buffer[5]=lowByte(targetTemp);
  neopixel_i2c_buffer[6]=highByte(maxTemp);
  neopixel_i2c_buffer[7]=lowByte(maxTemp);
  neopixel_i2c_buffer[8]=stripNumber;
  neopixel_i2c_buffer_length =9;
  send_neopixel_data();
}
void set_neopixel_heatbed(uint16_t currentTemp,uint16_t targetTemp, uint16_t maxTemp, uint8_t stripNumber){
  neopixel_i2c_buffer[0]='b';
  neopixel_i2c_buffer[1]=NEOPIXEL_STATE_BED_HEATUP;
  neopixel_i2c_buffer[2]=highByte(currentTemp);
  neopixel_i2c_buffer[3]=lowByte(currentTemp);
  neopixel_i2c_buffer[4]=highByte(targetTemp);
  neopixel_i2c_buffer[5]=lowByte(targetTemp);
  neopixel_i2c_buffer[6]=highByte(maxTemp);
  neopixel_i2c_buffer[7]=lowByte(maxTemp);
  neopixel_i2c_buffer[8]=stripNumber;
  neopixel_i2c_buffer_length =9;

  send_neopixel_data();
}
void set_neopixel_progress(uint8_t progress, uint8_t state, uint8_t stripNumber){
/* State:
  3: //Printing
  4: //Printing finished
  5: //Printing paused
  6: //Printing stopped */
  neopixel_i2c_buffer[0]='p';
  neopixel_i2c_buffer[1]=state; 
  neopixel_i2c_buffer[2]=progress;
  neopixel_i2c_buffer[3]=stripNumber;
  neopixel_i2c_buffer_length =4;
  send_neopixel_data();	
}
void set_neopixel_state(uint8_t state, uint8_t stripNumber){
/* State:
  3: //Printing
  4: //Printing finished
  5: //Printing paused
  6: //Printing stopped 
  9: //Idle */
  neopixel_i2c_buffer[0]='s';
  neopixel_i2c_buffer[1]=state; 
  neopixel_i2c_buffer[2]=stripNumber;
  neopixel_i2c_buffer_length =3;
  send_neopixel_data();	
}

millis_t next_send_delay_ms=0;
void send_neopixel_data(){
  millis_t now=millis();
  if (ELAPSED(now, next_send_delay_ms)) { 
    next_send_delay_ms = now + 50UL;
    Wire.setClock(400000);
    Wire.begin(); 
    Wire.beginTransmission(8); // transmit to device #8
    Wire.write(neopixel_i2c_buffer,neopixel_i2c_buffer_length);        // sends bytes
    Wire.endTransmission();
  }
}

#endif // NEOPIXEL_LED
