#ifndef DRAW_SCREEN_H
#define DRAW_SCREEN_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7796S.h>
#include "Car_Bitmaps.h"

#define TFT_CS   10
#define TFT_DC    9
#define TFT_RST   8

#define COLOR_BACKGROUND  ST77XX_WHITE   // Hintergrund
#define COLOR_CAR         ST77XX_BLACK   // Autolinien (bleiben immer in dieser Farbe)
#define COLOR_FAR         ST77XX_GREEN
#define COLOR_NEAR        0xFD20
#define COLOR_VERY_NEAR   ST77XX_RED

// Display-Korrekturen (bei Bedarf auf 0 setzen):
#define DISPLAY_INVERTED  1   // Farben sind invertiert -> Inversion einschalten
#define DISPLAY_SWAP_RB   1   // Rot und Blau sind vertauscht -> im Code tauschen

// Tauscht Rot- und Blauanteil einer RGB565-Farbe (Schwarz/Weiss/Gruen bleiben gleich).
static inline uint16_t fixColor(uint16_t c) {
#if DISPLAY_SWAP_RB
  return ((c & 0x001F) << 11) | (c & 0x07E0) | (c >> 11);
#else
  return c;
#endif
}

Adafruit_ST7796S tft(TFT_CS, TFT_DC, TFT_RST);

enum SensorZone {
  FRONT_LEFT = 0, FRONT_CENTER = 1, FRONT_RIGHT = 2,
  REAR_LEFT = 3, REAR_CENTER = 4, REAR_RIGHT = 5
};

enum SensorState {
  NO_OBJECT = 1, OBJECT_FAR = 2, OBJECT_NEAR = 3, OBJECT_VERY_NEAR = 4
};

uint8_t sensorState[6] = {
  NO_OBJECT, NO_OBJECT, NO_OBJECT,
  NO_OBJECT, NO_OBJECT, NO_OBJECT
};

uint16_t getSensorColor(uint8_t state) {
  switch (state) {
    case OBJECT_FAR: return fixColor(COLOR_FAR);
    case OBJECT_NEAR: return fixColor(COLOR_NEAR);
    case OBJECT_VERY_NEAR: return fixColor(COLOR_VERY_NEAR);
    default: return COLOR_CAR;
  }
}

// Zeichnet genau eine der sechs Zonen entsprechend ihrem aktuellen Zustand.
static void renderZone(uint8_t zone) {
  if (sensorState[zone] == NO_OBJECT) {
    drawCarZone(tft, zone, false, 0, COLOR_CAR, COLOR_BACKGROUND);
  } else {
    // Linien bleiben in COLOR_CAR, nur die Zone dahinter wird farbig.
    drawCarZone(tft, zone, true, getSensorColor(sensorState[zone]),
                COLOR_CAR, COLOR_BACKGROUND);
  }
}

void initScreen() {
  tft.init(320, 480);
  tft.setRotation(0);
  tft.invertDisplay(DISPLAY_INVERTED);
  tft.fillScreen(COLOR_BACKGROUND);
}

// Komplett neu zeichnen (z. B. beim Start).
void drawScreen() {
  tft.fillScreen(COLOR_BACKGROUND);
  for (uint8_t i = 0; i < 6; ++i) renderZone(i);
}

// Nur der angeforderte Sensorbereich wird neu geschrieben.
void updateSensor(uint8_t sensor, uint8_t state) {
  if (sensor > REAR_RIGHT || state < NO_OBJECT || state > OBJECT_VERY_NEAR)
    return;
  if (sensorState[sensor] == state) return;   // nichts zu tun, kein Flackern

  sensorState[sensor] = state;
  renderZone(sensor);
}

void setFrontLeft(uint8_t state)   { updateSensor(FRONT_LEFT, state); }
void setFrontCenter(uint8_t state) { updateSensor(FRONT_CENTER, state); }
void setFrontRight(uint8_t state)  { updateSensor(FRONT_RIGHT, state); }
void setRearLeft(uint8_t state)    { updateSensor(REAR_LEFT, state); }
void setRearCenter(uint8_t state)  { updateSensor(REAR_CENTER, state); }
void setRearRight(uint8_t state)   { updateSensor(REAR_RIGHT, state); }

#endif
