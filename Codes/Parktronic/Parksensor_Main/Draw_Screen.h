#ifndef DRAW_SCREEN_H
#define DRAW_SCREEN_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <avr/pgmspace.h>

#include "Car_Bitmaps.h"

// ============================================================
// Display configuration
#define TFT_CS   10
#define TFT_DC    9
#define TFT_RST   8

#define SCREEN_WIDTH   320
#define SCREEN_HEIGHT  240

// Each sensor area on the display
#define ZONE_WIDTH     160
#define ZONE_HEIGHT     80

#define BITMAP_SCALE     2

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);
// ============================================================


// ============================================================
// Sensor IDs
enum SensorZone{
  FRONT_LEFT = 0,
  FRONT_CENTER = 1,
  FRONT_RIGHT= 2,
  REAR_LEFT = 3,
  REAR_CENTER = 4,
  REAR_RIGHT = 5
};
// ============================================================


// ============================================================
// Sensor states
//
// 1 = no object
// 2 = object far away
// 3 = object near
// 4 = object very near
enum SensorState{
  NO_OBJECT = 1,
  OBJECT_FAR = 2,
  OBJECT_NEAR = 3,
  OBJECT_VERY_NEAR = 4
};
// ============================================================


uint8_t sensorState[6] = { // initialize all displayed Bitmaps with no Object (color = white)
  NO_OBJECT,
  NO_OBJECT,
  NO_OBJECT,
  NO_OBJECT,
  NO_OBJECT,
  NO_OBJECT
};


// ============================================================
// Colors
#define COLOR_BACKGROUND   ILI9341_BLACK 
#define COLOR_CAR          ILI9341_WHITE
#define COLOR_FAR          ILI9341_GREEN
#define COLOR_NEAR         0xFD20 // orange
#define COLOR_VERY_NEAR    ILI9341_RED
// ============================================================


// ============================================================
// Internal helper: get bitmap
const uint8_t* getCarBitmap(uint8_t sensor){
  switch (sensor){ // sensor = 0 -> 5
    case FRONT_LEFT: // 0
      return CAR_FRONT_LEFT;

    case FRONT_CENTER: // 1
      return CAR_FRONT_CENTER;

    case FRONT_RIGHT: // 2
      return CAR_FRONT_RIGHT;

    case REAR_LEFT: // 3
      return CAR_REAR_LEFT;

    case REAR_CENTER: // 4
      return CAR_REAR_CENTER;

    case REAR_RIGHT: // 5
      return CAR_REAR_RIGHT;
  }

  return CAR_FRONT_LEFT;
}
// ============================================================


// ============================================================
// Internal helper: get zone position
void getZonePosition(uint8_t sensor, int &x, int &y){ // get x and y position corresponding to sensor that sends data
  // Left half = front
  // Right half = rear

  if (sensor == FRONT_LEFT){ // sensor 0
    x = 0;
    y = 0;
  }
  else if (sensor == FRONT_CENTER){// sensor 1
    x = 0;
    y = 80;
  }
  else if (sensor == FRONT_RIGHT){// sensor 2
    x = 0;
    y = 160;
  }
  else if (sensor == REAR_LEFT){// sensor 3
    x = 160;
    y = 0;
  }
  else if (sensor == REAR_CENTER){// sensor 4
    x = 160;
    y = 80;}
  else{ // sensor 5
    x = 160;
    y = 160;
  }
}
// ============================================================


// ============================================================
// Internal helper: draw 80x40 bitmap at 2x scale
void drawCarBitmap(const uint8_t *bitmap, int x, int y){
  const int bytesPerRow = CAR_BITMAP_WIDTH / 8;

  for (int pos_y = 0; pos_y < CAR_BITMAP_HEIGHT; py++){
    for (int pos_x = 0; pos_x < CAR_BITMAP_WIDTH; px++){
      uint8_t byteValue = pgm_read_byte(
        &bitmap[pos_y * bytesPerRow + (pos_x / 8)]
      );

      if (byteValue & (0x80 >> (pos_x % 8))){
        tft.fillRect(
          x + pos_x * BITMAP_SCALE,
          y + pos_y * BITMAP_SCALE,
          BITMAP_SCALE,
          BITMAP_SCALE,
          COLOR_CAR
        );
      }
    }
  }
}
// ============================================================


// ============================================================
// Internal helper: draw warning indication
void drawWarning(uint8_t sensor, uint8_t state, int x, int y){
  if (state == NO_OBJECT)
    return; // no object = no warning

  uint16_t color; // color used for warning

  if (state == OBJECT_FAR) // state = Sensor State
    color = COLOR_FAR; // green
  else if (state == OBJECT_NEAR)
    color = COLOR_NEAR; // orange
  else
    color = COLOR_VERY_NEAR; // red

  /*
   * The warning bar is placed on the outside edge
   * of the corresponding sensor area.
   *
   * Front sensors -> left edge
   * Rear sensors  -> right edge
   */
  if (sensor == FRONT_LEFT || sensor == FRONT_CENTER || sensor == FRONT_RIGHT){
    tft.fillRect(
      x + 3,
      y + 5,
      10,
      ZONE_HEIGHT - 10,
      color
    );
  }
  else{
    tft.fillRect(
      x + ZONE_WIDTH - 13,
      y + 5,
      10,
      ZONE_HEIGHT - 10,
      color
    );
  }
}
// ============================================================


// ============================================================
// Initialize display
void initScreen(){
  tft.begin();

  // 1 = landscape, 320 x 240
  tft.setRotation(1);

  tft.fillScreen(COLOR_BACKGROUND);
}
// ============================================================


// ============================================================
// Draw one sensor area
//
// IMPORTANT:
// Only the selected 160 x 80 pixel area is cleared and drawn.
// The other five areas remain untouched.
void updateSensor(uint8_t sensor, uint8_t state){
  if (sensor > REAR_RIGHT)
    return;

  if (state < NO_OBJECT || state > OBJECT_VERY_NEAR)
    return;

  sensorState[sensor] = state;

  int x;
  int y;

  getZonePosition(sensor, x, y);

  // Clear ONLY this sensor's area.
  tft.fillRect(
    x,
    y,
    ZONE_WIDTH,
    ZONE_HEIGHT,
    COLOR_BACKGROUND
  );

  // Draw ONLY this sensor's bitmap.
  drawCarBitmap(
    getCarBitmap(sensor),
    x,
    y
  );

  // Draw the current warning state.
  drawWarning(
    sensor,
    state,
    x,
    y
  );
}
// ============================================================


// ============================================================
// Draw all six sensor areas
void drawScreen(){
  for (uint8_t sensor = 0; sensor < 6; sensor++){
    updateSensor(
      sensor,
      sensorState[sensor]
    );
  }
}
// ============================================================


// ============================================================
// Convenience functions
void setFrontLeft(uint8_t state){
  updateSensor(FRONT_LEFT, state);
}

void setFrontCenter(uint8_t state){
  updateSensor(FRONT_CENTER, state);
}

void setFrontRight(uint8_t state){
  updateSensor(FRONT_RIGHT, state);
}

void setRearLeft(uint8_t state){
  updateSensor(REAR_LEFT, state);
}

void setRearCenter(uint8_t state){
  updateSensor(REAR_CENTER, state);
}

void setRearRight(uint8_t state){
  updateSensor(REAR_RIGHT, state);
}
// ============================================================

#endif