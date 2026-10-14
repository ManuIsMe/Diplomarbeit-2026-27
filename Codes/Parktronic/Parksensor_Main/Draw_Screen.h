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
// Physical display: 240 x 320 pixels
// Used horizontally (landscape): 320 x 240 pixels
// ============================================================
#define TFT_CS   10
#define TFT_DC    9
#define TFT_RST   8

#define SCREEN_WIDTH   320
#define SCREEN_HEIGHT  240

// Six independent areas:
// 160 x 80 pixels each
#define ZONE_WIDTH     160
#define ZONE_HEIGHT     80

// Car_Bitmaps.h contains 80 x 40 bitmaps.
// They are displayed at 2x scale -> 160 x 80 pixels.
#define BITMAP_SCALE    2

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

// ============================================================
// Sensor IDs
// ============================================================
enum SensorZone {
  FRONT_LEFT   = 0,
  FRONT_CENTER = 1,
  FRONT_RIGHT  = 2,
  REAR_LEFT    = 3,
  REAR_CENTER  = 4,
  REAR_RIGHT   = 5
};

// ============================================================
// Sensor states
// ============================================================
// 1 = no object
// 2 = object far away
// 3 = object near
// 4 = object very near
// ============================================================
enum SensorState {
  NO_OBJECT       = 1,
  OBJECT_FAR      = 2,
  OBJECT_NEAR     = 3,
  OBJECT_VERY_NEAR = 4
};

// Current state of all six sensors.
uint8_t sensorState[6] = {
  NO_OBJECT,
  NO_OBJECT,
  NO_OBJECT,
  NO_OBJECT,
  NO_OBJECT,
  NO_OBJECT
};

// ============================================================
// Colors
// ============================================================
#define COLOR_BACKGROUND   ILI9341_BLACK
#define COLOR_CAR          ILI9341_WHITE
#define COLOR_FAR          ILI9341_GREEN
#define COLOR_NEAR         0xFD20   // orange
#define COLOR_VERY_NEAR    ILI9341_RED

// ============================================================
// Get bitmap belonging to a sensor
// ============================================================
const uint8_t* getCarBitmap(uint8_t sensor) {
  switch (sensor) {
    case FRONT_LEFT:
      return CAR_FRONT_LEFT;

    case FRONT_CENTER:
      return CAR_FRONT_CENTER;

    case FRONT_RIGHT:
      return CAR_FRONT_RIGHT;

    case REAR_LEFT:
      return CAR_REAR_LEFT;

    case REAR_CENTER:
      return CAR_REAR_CENTER;

    case REAR_RIGHT:
      return CAR_REAR_RIGHT;
  }

  return CAR_FRONT_LEFT;
}

// ============================================================
// Get position of a sensor area
//
// Display in landscape:
//
// 320 x 240
//
// +----------------+----------------+
// | FRONT_LEFT     | REAR_LEFT      | 80
// +----------------+----------------+
// | FRONT_CENTER   | REAR_CENTER    | 80
// +----------------+----------------+
// | FRONT_RIGHT    | REAR_RIGHT     | 80
// +----------------+----------------+
//        160              160
//
// Front is on the LEFT, rear is on the RIGHT.
// ============================================================
void getZonePosition(uint8_t sensor, int &x, int &y) {
  switch (sensor) {
    case FRONT_LEFT:
      x = 0;
      y = 0;
      break;

    case FRONT_CENTER:
      x = 0;
      y = 80;
      break;

    case FRONT_RIGHT:
      x = 0;
      y = 160;
      break;

    case REAR_LEFT:
      x = 160;
      y = 0;
      break;

    case REAR_CENTER:
      x = 160;
      y = 80;
      break;

    case REAR_RIGHT:
      x = 160;
      y = 160;
      break;

    default:
      x = 0;
      y = 0;
      break;
  }
}

// ============================================================
// Draw one 80 x 40 bitmap at 2x scale.
// Result: 160 x 80 pixels.
// ============================================================
void drawCarBitmap(const uint8_t *bitmap, int x, int y) {
  const uint8_t bytesPerRow = CAR_BITMAP_WIDTH / 8;

  for (int pos_y = 0; pos_y < CAR_BITMAP_HEIGHT; pos_y++) {
    for (int pos_x = 0; pos_x < CAR_BITMAP_WIDTH; pos_x++) {

      uint8_t byteValue = pgm_read_byte(
        &bitmap[pos_y * bytesPerRow + (pos_x / 8)]
      );

      if (byteValue & (0x80 >> (pos_x % 8))) {
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
// Draw warning indication for one sensor
// ============================================================
void drawWarning(uint8_t sensor, uint8_t state, int x, int y) {
  if (state == NO_OBJECT) {
    return;
  }

  uint16_t color;

  if (state == OBJECT_FAR) {
    color = COLOR_FAR;
  }
  else if (state == OBJECT_NEAR) {
    color = COLOR_NEAR;
  }
  else {
    color = COLOR_VERY_NEAR;
  }

  // Front sensors -> warning bar on the left edge.
  // Rear sensors  -> warning bar on the right edge.
  if (sensor == FRONT_LEFT ||
      sensor == FRONT_CENTER ||
      sensor == FRONT_RIGHT) {

    tft.fillRect(
      x + 3,
      y + 5,
      10,
      ZONE_HEIGHT - 10,
      color
    );
  }
  else {
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
// Initialize display
// ============================================================
void initScreen() {
  tft.begin();

  // Physical display is 240 x 320.
  // Rotation 1 makes the usable coordinate system 320 x 240.
  tft.setRotation(1);

  tft.fillScreen(COLOR_BACKGROUND);
}

// ============================================================
// Update exactly ONE sensor area
//
// Only this 160 x 80 area is cleared and redrawn.
// The other five areas are not touched.
// ============================================================
void updateSensor(uint8_t sensor, uint8_t state) {
  if (sensor > REAR_RIGHT) {
    return;
  }

  if (state < NO_OBJECT || state > OBJECT_VERY_NEAR) {
    return;
  }

  sensorState[sensor] = state;

  int x;
  int y;
  getZonePosition(sensor, x, y);

  // Clear ONLY this sensor area.
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

  // Draw warning state.
  drawWarning(
    sensor,
    state,
    x,
    y
  );
}

// ============================================================
// Draw all six areas
// ============================================================
void drawScreen() {
  for (uint8_t sensor = 0; sensor < 6; sensor++) {
    updateSensor(sensor, sensorState[sensor]);
  }
}

// ============================================================
// Convenience functions
// ============================================================
void setFrontLeft(uint8_t state) {
  updateSensor(FRONT_LEFT, state);
}

void setFrontCenter(uint8_t state) {
  updateSensor(FRONT_CENTER, state);
}

void setFrontRight(uint8_t state) {
  updateSensor(FRONT_RIGHT, state);
}

void setRearLeft(uint8_t state) {
  updateSensor(REAR_LEFT, state);
}

void setRearCenter(uint8_t state) {
  updateSensor(REAR_CENTER, state);
}

void setRearRight(uint8_t state) {
  updateSensor(REAR_RIGHT, state);
}

#endif
