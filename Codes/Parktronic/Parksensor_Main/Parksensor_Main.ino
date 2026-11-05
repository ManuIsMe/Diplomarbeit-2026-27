#include "Draw_Screen.h"
#include <avr/interrupt.h>
/*
  Sensor Pins:
    Front Center:
      Trigger -> A0
      Echo -> A1
    Front Left:
      Trigger -> A2
      Echo -> A3
    Front Right:
      Trigger -> A4
      Echo -> A5

    Rear Center:
      Trigger -> 0
      Echo -> 1
    Rear Left:
      Trigger -> 2
      Echo -> 4
    Rear Right:
      Trigger -> 5
      Echo -> 7

  Display Pins:
    Display VCC = 5V
    Display LED Pin for background lighting = 3.3V
    RESET -> 8
    DC -> 9
    CS -> 10
    MOSI -> 11  
    SCK -> 13  

  Speaker Pins:
    Left speaker -> 3
    Right speaker -> 6
*/

// Pins for front center sensor
#define TRIG_PIN1 A0
#define ECHO_PIN1 A1
// Pins for front left sensor
#define TRIG_PIN2 A2
#define ECHO_PIN2 A3
// Pins for front right sensor
#define TRIG_PIN3 A4
#define ECHO_PIN3 A5
// Pins for rear center sensor
#define TRIG_PIN4 0
#define ECHO_PIN4 1
// Pins for rear left sensor
#define TRIG_PIN5 2
#define ECHO_PIN5 4
// Pins for rear right sensor
#define TRIG_PIN6 5
#define ECHO_PIN6 7

// Beepstates
enum BeepState {
  NO_Object = 1, Object_FAR = 2, Object_NEAR = 3, Object_VERY_NEAR = 4
};

//Pins and variables  speaker
#define SPEAKER_PIN_LEFT 3
#define SPEAKER_PIN_RIGHT 6
unsigned long lastBeep_right = 0;
unsigned long lastBeep_left = 0;
unsigned long beepStart_right = 0;
unsigned long beepStart_left = 0;
uint8_t lastBeepState_right = NO_Object;
uint8_t lastBeepState_left = NO_Object;
bool beepActive_right = false;
bool beepActive_left = false;
const uint8_t BEEP_DURATION = 50;
volatile bool speakerTone_right = false;
volatile bool speakerTone_left = false;

// Remaining beep duration in milliseconds
volatile uint8_t speakerTicks_right = 0;
volatile uint8_t speakerTicks_left = 0;

// All sensor states initialized with NO_OBJECT
int Front_Center_State = 0;  
int Front_Left_State = 0;
int Front_Right_State = 0;
int Rear_Center_State = 0;
int Rear_Left_State = 0;
int Rear_Right_State = 0;

// All Last... states are initialized with NO_OBJECT
int Last_Front_Center_State = 0; 
int Last_Front_Left_State = 0; 
int Last_Front_Right_State = 0; 
int Last_Rear_Center_State = 0; 
int Last_Rear_Left_State = 0; 
int Last_Rear_Right_State = 0; 

// Variables for distance
// All distances initialized with 999 (NO_OBJECT)
float distance_rear_center = 999;
float distance_rear_left = 999;
float distance_rear_right = 999;

float distance_front_center = 999;
float distance_front_left = 999;
float distance_front_right = 999;

void setup() {
  // Start Display:
  initScreen();
  drawScreen();

  // Sensor Pins:
  pinMode(TRIG_PIN1, OUTPUT);
  pinMode(ECHO_PIN1, INPUT);
  pinMode(TRIG_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);
  pinMode(TRIG_PIN3, OUTPUT);
  pinMode(ECHO_PIN3, INPUT);
  pinMode(TRIG_PIN4, OUTPUT);
  pinMode(ECHO_PIN4, INPUT);
  pinMode(TRIG_PIN5, OUTPUT);
  pinMode(ECHO_PIN5, INPUT);
  pinMode(TRIG_PIN6, OUTPUT);
  pinMode(ECHO_PIN6, INPUT);

  digitalWrite(TRIG_PIN1, LOW);
  digitalWrite(TRIG_PIN2, LOW);
  digitalWrite(TRIG_PIN3, LOW);
  digitalWrite(TRIG_PIN4, LOW);
  digitalWrite(TRIG_PIN5, LOW);
  digitalWrite(TRIG_PIN6, LOW);

 // Speaker pins
  pinMode(SPEAKER_PIN_LEFT, OUTPUT);
  pinMode(SPEAKER_PIN_RIGHT, OUTPUT);

  // Timer1 configuration
  noInterrupts();

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // 1000 interrupts per second
  OCR1A = 1999;

  TCCR1B |= (1 << WGM12);  // CTC mode
  TCCR1B |= (1 << CS11);   // Prescaler 8
  TIMSK1 |= (1 << OCIE1A); // Enable Timer1 interrupt

  interrupts();
}

void loop() {
  Front_Center();
  Front_Left();
  Front_Right();

  Rear_Center();
  Rear_Left();
  Rear_Right();

  updateSpeakers();
}

// Functions for all sensors
void Front_Center(){
  // Trigger-Puls
  digitalWrite(TRIG_PIN1, LOW);
  delayMicroseconds(5);

  digitalWrite(TRIG_PIN1, HIGH);
  delayMicroseconds(20);
  digitalWrite(TRIG_PIN1, LOW);

  // Wait for echo
  unsigned long time = pulseIn(ECHO_PIN1, HIGH, 24000);

  if (time > 0){
    float distance = time * 0.0343 / 2.0;

    if (distance <= 35){      
      distance_front_center = distance;
      Front_Center_State = 3;

      // Update Front Center Display State
      if (Last_Front_Center_State != Front_Center_State){
        updateSensor(FRONT_CENTER, OBJECT_VERY_NEAR);
        Last_Front_Center_State = 3; 
      }
    } else if (distance > 35 && distance <= 55){
      distance_front_center = distance;
      Front_Center_State = 2;

      // Update Front Center Display State
      if (Last_Front_Center_State != Front_Center_State){
        updateSensor(FRONT_CENTER, OBJECT_NEAR);
        Last_Front_Center_State = 2; 
      }
    } else if (distance > 55 && distance <= 100){
      distance_front_center = distance;
      Front_Center_State = 1;

      // Update Front Center Display State
      if (Last_Front_Center_State != Front_Center_State){
        updateSensor(FRONT_CENTER, OBJECT_FAR);
        Last_Front_Center_State = 1; 
      }
    } else{
      Front_Center_State = 0;
      distance_front_center = 999;

      // Update Front Center Display State
      if (Last_Front_Center_State != Front_Center_State){
        updateSensor(FRONT_CENTER, NO_OBJECT);
        Last_Front_Center_State = 0; 
      }
    }
  }
  else {
    distance_front_center = 999;
    Front_Center_State = 0;

    if (Last_Front_Center_State != Front_Center_State) {
      updateSensor(FRONT_CENTER, NO_OBJECT);
      Last_Front_Center_State = 0;
    }
  }
}
void Front_Left(){
  // Trigger-Puls
  digitalWrite(TRIG_PIN2, LOW);
  delayMicroseconds(5);

  digitalWrite(TRIG_PIN2, HIGH);
  delayMicroseconds(20);
  digitalWrite(TRIG_PIN2, LOW);

  // Wait for echo
  unsigned long time = pulseIn(ECHO_PIN2, HIGH, 24000);

  if (time > 0){
    float distance = time * 0.0343 / 2.0;

    if (distance <= 35){      
      distance_front_left = distance;
      Front_Left_State = 3;

      // Update Front left Display State
      if (Last_Front_Left_State != Front_Left_State){
        updateSensor(FRONT_LEFT, OBJECT_VERY_NEAR);
        Last_Front_Left_State = 3; 
      }
    } else if (distance > 35 && distance <= 55){
      distance_front_left = distance;
      Front_Left_State = 2;

      // Update Front left Display State
      if (Last_Front_Left_State != Front_Left_State){
        updateSensor(FRONT_LEFT, OBJECT_NEAR);
        Last_Front_Left_State = 2; 
      }
    } else if (distance > 55 && distance <= 100){
      distance_front_left = distance;
      Front_Left_State = 1;

      // Update Front left Display State
      if (Last_Front_Left_State != Front_Left_State){
        updateSensor(FRONT_LEFT, OBJECT_FAR);
        Last_Front_Left_State = 1; 
      }
    } else{
      Front_Left_State = 0;
      distance_front_left = 999;

      // Update Front left Display State
      if (Last_Front_Left_State != Front_Left_State){
        updateSensor(FRONT_LEFT, NO_OBJECT);
        Last_Front_Left_State = 0; 
      }
    }
  }
  else {
    distance_front_left = 999;
    Front_Left_State = 0;

    if (Last_Front_Left_State != Front_Left_State) {
      updateSensor(FRONT_LEFT, NO_OBJECT);
      Last_Front_Left_State = 0;
    }
  }
}
void Front_Right(){
  // Trigger-Puls
  digitalWrite(TRIG_PIN3, LOW);
  delayMicroseconds(5);

  digitalWrite(TRIG_PIN3, HIGH);
  delayMicroseconds(20);
  digitalWrite(TRIG_PIN3, LOW);

  // Wait for echo
  unsigned long time = pulseIn(ECHO_PIN3, HIGH, 24000);

  if (time > 0){
    float distance = time * 0.0343 / 2.0;

    if (distance <= 35){
      distance_front_right = distance;
      Front_Right_State = 3;

      // Update Front right Display State
      if (Last_Front_Right_State != Front_Right_State){
        updateSensor(FRONT_RIGHT, OBJECT_VERY_NEAR);
        Last_Front_Right_State = 3; 
      }
    } else if (distance > 35 && distance <= 55){
      distance_front_right = distance;
      Front_Right_State = 2;

      // Update Front right Display State
      if (Last_Front_Right_State != Front_Right_State){
        updateSensor(FRONT_RIGHT, OBJECT_NEAR);
        Last_Front_Right_State = 2; 
      }
    } else if (distance > 55 && distance <= 100){
      distance_front_right = distance;
      Front_Right_State = 1;

      // Update Front right Display State
      if (Last_Front_Right_State != Front_Right_State){
        updateSensor(FRONT_RIGHT, OBJECT_FAR);
        Last_Front_Right_State = 1; 
      }
    } else{
      Front_Right_State = 0;
      distance_front_right = 999;

      // Update Front right Display State
      if (Last_Front_Right_State != Front_Right_State){
        updateSensor(FRONT_RIGHT, NO_OBJECT);
        Last_Front_Right_State = 0; 
      }
    }
  }
  else {
    distance_front_right = 999;
    Front_Right_State = 0;

    if (Last_Front_Right_State != Front_Right_State) {
      updateSensor(FRONT_RIGHT, NO_OBJECT);
      Last_Front_Right_State = 0;
    }
  }
}

void Rear_Center(){
  // Trigger-Puls
  digitalWrite(TRIG_PIN4, LOW);
  delayMicroseconds(5);

  digitalWrite(TRIG_PIN4, HIGH);
  delayMicroseconds(20);
  digitalWrite(TRIG_PIN4, LOW);

  // Wait for echo
  unsigned long time = pulseIn(ECHO_PIN4, HIGH, 24000);

  if (time > 0){
    float distance = time * 0.0343 / 2.0;

    if (distance <= 35){
      distance_rear_center = distance;
      Rear_Center_State = 3;

      // Update Rear center Display State
      if (Last_Rear_Center_State != Rear_Center_State){
        updateSensor(REAR_CENTER, OBJECT_VERY_NEAR);
        Last_Rear_Center_State = 3; 
      }
    } else if (distance > 35 && distance <= 55){
      distance_rear_center = distance;
      Rear_Center_State = 2;

      // Update Rear center Display State
      if (Last_Rear_Center_State != Rear_Center_State){
        updateSensor(REAR_CENTER, OBJECT_NEAR);
        Last_Rear_Center_State = 2; 
      }
    } else if (distance > 55 && distance <= 100){
      distance_rear_center = distance;
      Rear_Center_State = 1;

      // Update Rear center Display State
      if (Last_Rear_Center_State != Rear_Center_State){
        updateSensor(REAR_CENTER, OBJECT_FAR);
        Last_Rear_Center_State = 1; 
      }
    } else{
      Rear_Center_State = 0;
      distance_rear_center = 999;

      // Update Rear center Display State
      if (Last_Rear_Center_State != Rear_Center_State){
        updateSensor(REAR_CENTER, NO_OBJECT);
        Last_Rear_Center_State = 0; 
      }
    }
  }
  else {
    distance_rear_center = 999;
    Rear_Center_State = 0;

    if (Last_Rear_Center_State != Rear_Center_State) {
      updateSensor(REAR_CENTER, NO_OBJECT);
      Last_Rear_Center_State = 0;
    }
  }
}
void Rear_Left(){
  // Trigger-Puls
  digitalWrite(TRIG_PIN5, LOW);
  delayMicroseconds(5);

  digitalWrite(TRIG_PIN5, HIGH);
  delayMicroseconds(20);
  digitalWrite(TRIG_PIN5, LOW);

  // Wait for echo
  unsigned long time = pulseIn(ECHO_PIN5, HIGH, 24000);

  if (time > 0){
    float distance = time * 0.0343 / 2.0;

    if (distance <= 35){
      distance_rear_left = distance;
      Rear_Left_State = 3;

      // Update Rear left Display State
      if (Last_Rear_Left_State != Rear_Left_State){
        updateSensor(REAR_LEFT, OBJECT_VERY_NEAR);
        Last_Rear_Left_State = 3; 
      }
    } else if (distance > 35 && distance <= 55){
      distance_rear_left = distance;
      Rear_Left_State = 2;

      // Update Rear left Display State
      if (Last_Rear_Left_State != Rear_Left_State){
        updateSensor(REAR_LEFT, OBJECT_NEAR);
        Last_Rear_Left_State = 2; 
      }
    } else if (distance > 55 && distance <= 100){
      distance_rear_left = distance;
      Rear_Left_State = 1;

      // Update Rear left Display State
      if (Last_Rear_Left_State != Rear_Left_State){
        updateSensor(REAR_LEFT, OBJECT_FAR);
        Last_Rear_Left_State = 1; 
      }
    } else{
      Rear_Left_State = 0;
      distance_rear_left = 999;

      // Update Rear left Display State
      if (Last_Rear_Left_State != Rear_Left_State){
        updateSensor(REAR_LEFT, NO_OBJECT);
        Last_Rear_Left_State = 0; 
      }
    }
  }
  else {
    distance_rear_left = 999;
    Rear_Left_State = 0;

    if (Last_Rear_Left_State != Rear_Left_State) {
      updateSensor(REAR_LEFT, NO_OBJECT);
      Last_Rear_Left_State = 0;
    }
  }
}
void Rear_Right(){
  // Trigger-Puls
  digitalWrite(TRIG_PIN6, LOW);
  delayMicroseconds(5);

  digitalWrite(TRIG_PIN6, HIGH);
  delayMicroseconds(20);
  digitalWrite(TRIG_PIN6, LOW);

  // Wait for echo
  unsigned long time = pulseIn(ECHO_PIN6, HIGH, 24000);

  if (time > 0){
    float distance = time * 0.0343 / 2.0;

    if (distance <= 35){
      distance_rear_right = distance;
      Rear_Right_State = 3;

      // Update Rear right Display State
      if (Last_Rear_Right_State != Rear_Right_State){
        updateSensor(REAR_RIGHT, OBJECT_VERY_NEAR);
        Last_Rear_Right_State = 3; 
      }
    } else if (distance > 35 && distance <= 55){
      distance_rear_right = distance;
      Rear_Right_State = 2;

      // Update Rear right Display State
      if (Last_Rear_Right_State != Rear_Right_State){
        updateSensor(REAR_RIGHT, OBJECT_NEAR);
        Last_Rear_Right_State = 2; 
      }
    } else if (distance > 55 && distance <= 100){
      distance_rear_right = distance;
      Rear_Right_State = 1;

      // Update Rear right Display State
      if (Last_Rear_Right_State != Rear_Right_State){
        updateSensor(REAR_RIGHT, OBJECT_FAR);
        Last_Rear_Right_State = 1; 
      }
    } else{
      Rear_Right_State = 0;
      distance_rear_right = 999;

      // Update Rear right Display State
      if (Last_Rear_Right_State != Rear_Right_State){
        updateSensor(REAR_RIGHT, NO_OBJECT);
        Last_Rear_Right_State = 0; 
      }
    }
  }
  else {
    distance_rear_right = 999;
    Rear_Right_State = 0;

    if (Last_Rear_Right_State != Rear_Right_State) {
      updateSensor(REAR_RIGHT, NO_OBJECT);
      Last_Rear_Right_State = 0;
    }
  }
}

// Method to find minimum distance
float getMinimumDistance(float d1, float d2, float d3, float d4) {
    float minDistance = d1;

    if (d2 < minDistance) minDistance = d2;
    if (d3 < minDistance) minDistance = d3;
    if (d4 < minDistance) minDistance = d4;

    return minDistance;
}

// Method to return Beepstates
uint8_t getBeepState(float distance) {

    if (distance <= 35) {
        return Object_VERY_NEAR;
    }
    else if (distance <= 55) {
        return Object_NEAR;
    }
    else if (distance <= 100) {
        return Object_FAR;
    }
    else {
        return NO_Object;
    }
}

// Mehod to update speakers to beep
void updateSpeakers() {
    // Minimum distance for left speaker
    float leftDistance = getMinimumDistance(
        distance_front_left,
        distance_rear_left,
        distance_front_center,
        distance_rear_center
    );

    // Minimum distance for right speaker
    float rightDistance = getMinimumDistance(
        distance_front_right,
        distance_rear_right,
        distance_front_center,
        distance_rear_center
    );

    // Update left speaker
    updateBeep_left(getBeepState(leftDistance));

    // Update right speaker
    updateBeep_right(getBeepState(rightDistance));
}

// Method to make right speaker beep
void updateBeep_right(uint8_t BeepState) {
  unsigned long currentTime = millis();
  unsigned long beepInterval_right;

  if (BeepState == Object_VERY_NEAR) {
    beepInterval_right = 150;
  }
  else if (BeepState == Object_NEAR) {
    beepInterval_right = 400;
  }
  else if (BeepState == Object_FAR) {
    beepInterval_right = 800;
  }
  else {
    speakerTone_right = false;
    speakerTicks_right = 0;
    beepActive_right = false;
    lastBeepState_right = NO_Object;
    return;
  }

  // Detect a change in distance category
  if (BeepState != lastBeepState_right) {
    lastBeepState_right = BeepState;
    lastBeep_right = currentTime;
  }

  // Update beep status after 50 ms
  if (beepActive_right &&
      currentTime - beepStart_right >= BEEP_DURATION) {

    beepActive_right = false;
  }

  // Start next beep
  if (!beepActive_right &&
      currentTime - lastBeep_right >= beepInterval_right) {

    noInterrupts();
    speakerTicks_right = BEEP_DURATION;
    speakerTone_right = true;
    interrupts();

    beepStart_right = currentTime;
    lastBeep_right = currentTime;
    beepActive_right = true;
  }
}

// Method to make left speaker beep
void updateBeep_left(uint8_t BeepState) {
  unsigned long currentTime = millis();
  unsigned long beepInterval_left;

  if (BeepState == Object_VERY_NEAR) {
    beepInterval_left = 150;
  }
  else if (BeepState == Object_NEAR) {
    beepInterval_left = 400;
  }
  else if (BeepState == Object_FAR) {
    beepInterval_left = 800;
  }
  else {
    speakerTone_left = false;
    speakerTicks_left = 0;
    beepActive_left = false;
    lastBeepState_left = NO_Object;
    return;
  }

  // Detect a change in distance category
  if (BeepState != lastBeepState_left) {
    lastBeepState_left = BeepState;
    lastBeep_left = currentTime;
  }

  // Update beep status after 50 ms
  if (beepActive_left &&
      currentTime - beepStart_left >= BEEP_DURATION) {

    beepActive_left = false;
  }

  // Start next beep
  if (!beepActive_left &&
      currentTime - lastBeep_left >= beepInterval_left) {

    noInterrupts();
    speakerTicks_left = BEEP_DURATION;
    speakerTone_left = true;
    interrupts();

    beepStart_left = currentTime;
    lastBeep_left = currentTime;
    beepActive_left = true;
  }
}

// interrupt handler for simultanious beeps
ISR(TIMER1_COMPA_vect) {
  static bool phase = false;

  // Count down beep duration (1 ms per interrupt)
  if (speakerTicks_left > 0) {
    speakerTicks_left--;

    if (speakerTicks_left == 0) {
      speakerTone_left = false;
    }
  }

  if (speakerTicks_right > 0) {
    speakerTicks_right--;

    if (speakerTicks_right == 0) {
      speakerTone_right = false;
    }
  }

  // Generate 500 Hz square wave
  phase = !phase;

  // Left speaker
  if (speakerTone_left && phase) {
    PORTD |= (1 << PD3);
  } else {
    PORTD &= ~(1 << PD3);
  }

  // Right speaker
  if (speakerTone_right && phase) {
    PORTD |= (1 << PD6);
  } else {
    PORTD &= ~(1 << PD6);
  }
}