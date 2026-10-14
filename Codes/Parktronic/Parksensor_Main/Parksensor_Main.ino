#include "Draw_Screen.h"

#define LED_GREEN A2
#define LED_YELLOW A1
#define LED_RED A0

// Sensor 1:
#define TRIG_PIN A3
#define ECHO_PIN A4

#define SPEAKER_PIN 3
int frequency = 500;
unsigned long lastBeep = 0;
bool beepActive = false;
const unsigned long BEEP_DURATION = 50;

void setup() {
  Serial.begin(9600);

  // Start Display:
  initScreen();
  drawScreen();

  // Sensor Pins:
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(TRIG_PIN, LOW);

  // LED-Pins:
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);

  // Startup:
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  delay(100);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_RED, LOW);
  delay(100);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, HIGH);
  delay(100);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_RED, LOW);
  delay(100);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  delay(100);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
}

void loop() {
  // Trigger-Puls
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(20);
  digitalWrite(TRIG_PIN, LOW);

  // Echo abwarten
  unsigned long time = pulseIn(ECHO_PIN, HIGH, 24000);

  if (time > 0){
    float distance = time * 0.0343 / 2.0;

    if (distance <= 35){
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_RED, HIGH);
      
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep(distance, frequency);
    } else if (distance > 35 && distance <= 55){
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_YELLOW, HIGH);
      digitalWrite(LED_RED, LOW);
      
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep(distance, frequency);
    } else if (distance > 55 && distance <= 100){
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_RED, LOW);
      
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep(distance, frequency);
    } else{
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_RED, LOW);
      
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      noTone(SPEAKER_PIN);
    }
  }
}


void updateBeep(float distance, int frequency) {
  unsigned long currentTime = millis();
  unsigned long beepInterval;

  // Abstand -> Geschwindigkeit
  if (distance <= 35) {
    beepInterval = 150;
  }
  else if (distance > 35 && distance <= 55) {
    beepInterval = 400;
  }
  else if (distance > 55 && distance <= 100) {
    beepInterval = 800;
  }
  else {
    noTone(SPEAKER_PIN);
    beepActive = false;
    return;
  }

  // Piepton starten
  if (!beepActive &&
      currentTime - lastBeep >= beepInterval) {
    lastBeep = currentTime;
    tone(SPEAKER_PIN, frequency);
    beepActive = true;
  }

  // Piepton beenden
  if (beepActive &&
      currentTime - lastBeep >= BEEP_DURATION) {
    noTone(SPEAKER_PIN);
    beepActive = false;
  }
}