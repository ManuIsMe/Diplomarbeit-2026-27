#include "Draw_Screen.h"
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

    Back Center:
      Trigger -> 0
      Echo -> 1
    Back Left:
      Trigger -> 2
      Echo -> 4
    Back Right:
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
// Pins for back center sensor
#define TRIG_PIN4 0
#define ECHO_PIN4 1
// Pins for back left sensor
#define TRIG_PIN5 2
#define ECHO_PIN5 4
// Pins for back right sensor
#define TRIG_PIN6 5
#define ECHO_PIN6 7

//Pins and variables  speaker
#define SPEAKER_PIN_LEFT 3
#define SPEAKER_PIN_RIGHT 6
int frequency = 500;
unsigned long lastBeep_right = 0;
unsigned long lastBeep_left = 0;
bool beepActive_right = false;
bool beepActive_left = false;
const unsigned long BEEP_DURATION = 50;

// All sensor states initialized with No_Object
int Front_Center_State = 0;  
int Front_Left_State = 0;
int Front_Right_State = 0;
int Back_Center_State = 0;
int Back_Left_State = 0;
int Back_Right_State = 0;

// All Last... states are set to a not defined state at initialization
int Last_Front_Center_State = 5; 
int Last_Front_Left_State = 5; 
int Last_Front_Right_State = 5; 
int Last_Back_Center_State = 5; 
int Last_Back_Left_State = 5; 
int Last_Back_Right_State = 5; 

void setup() {
  //Serial.begin(9600);

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
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep_left(distance, frequency);
      updateBeep_right(distance, frequency);
      Front_Center_State = 3;

      // Update Front Center Display State
      if (Last_Front_Center_State != Front_Center_State){
        updateSensor(FRONT_CENTER, OBJECT_VERY_NEAR);
        Last_Front_Center_State = 3; 
      }
    } else if (distance > 35 && distance <= 55){
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep_left(distance, frequency);
      updateBeep_right(distance, frequency);
      Front_Center_State = 2;

      // Update Front Center Display State
      if (Last_Front_Center_State != Front_Center_State){
        updateSensor(FRONT_CENTER, OBJECT_NEAR);
        Last_Front_Center_State = 2; 
      }
    } else if (distance > 55 && distance <= 100){
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep_left(distance, frequency);
      updateBeep_right(distance, frequency);
      Front_Center_State = 1;

      // Update Front Center Display State
      if (Last_Front_Center_State != Front_Center_State){
        updateSensor(FRONT_CENTER, OBJECT_FAR);
        Last_Front_Center_State = 1; 
      }
    } else{
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      noTone(SPEAKER_PIN_LEFT);
      noTone(SPEAKER_PIN_RIGHT);
      Front_Center_State = 0;

      // Update Front Center Display State
      if (Last_Front_Center_State != Front_Center_State){
        updateSensor(FRONT_CENTER, NO_OBJECT);
        Last_Front_Center_State = 0; 
      }
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
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep_left(distance, frequency);
      Front_Left_State = 3;

      // Update Front left Display State
      if (Last_Front_Left_State != Front_Left_State){
        updateSensor(FRONT_LEFT, OBJECT_VERY_NEAR);
        Last_Front_Left_State = 3; 
      }
    } else if (distance > 35 && distance <= 55){
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep_left(distance, frequency);
      Front_Left_State = 2;

      // Update Front left Display State
      if (Last_Front_Left_State != Front_Left_State){
        updateSensor(FRONT_LEFT, OBJECT_NEAR);
        Last_Front_Left_State = 2; 
      }
    } else if (distance > 55 && distance <= 100){
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep_left(distance, frequency);
      Front_Left_State = 1;

      // Update Front left Display State
      if (Last_Front_Left_State != Front_Left_State){
        updateSensor(FRONT_LEFT, OBJECT_FAR);
        Last_Front_Left_State = 1; 
      }
    } else{
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      noTone(SPEAKER_PIN_LEFT);
      Front_Left_State = 0;

      // Update Front left Display State
      if (Last_Front_Left_State != Front_Left_State){
        updateSensor(FRONT_LEFT, NO_OBJECT);
        Last_Front_Left_State = 0; 
      }
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
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep_right(distance, frequency);
      Front_Right_State = 3;

      // Update Front right Display State
      if (Last_Front_Right_State != Front_Right_State){
        updateSensor(FRONT_RIGHT, OBJECT_VERY_NEAR);
        Last_Front_Right_State = 3; 
      }
    } else if (distance > 35 && distance <= 55){
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep_right(distance, frequency);
      Front_Right_State = 2;

      // Update Front right Display State
      if (Last_Front_Right_State != Front_Right_State){
        updateSensor(FRONT_RIGHT, OBJECT_NEAR);
        Last_Front_Right_State = 2; 
      }
    } else if (distance > 55 && distance <= 100){
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep_right(distance, frequency);
      Front_Right_State = 1;

      // Update Front right Display State
      if (Last_Front_Right_State != Front_Right_State){
        updateSensor(FRONT_RIGHT, OBJECT_FAR);
        Last_Front_Right_State = 1; 
      }
    } else{
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      noTone(SPEAKER_PIN_RIGHT);
      Front_Right_State = 0;

      // Update Front right Display State
      if (Last_Front_Right_State != Front_Right_State){
        updateSensor(FRONT_RIGHT, NO_OBJECT);
        Last_Front_Right_State = 0; 
      }
    }
  }
}

void Back_Center(){
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
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep_left(distance, frequency);
      updateBeep_right(distance, frequency);
      Back_Center_State = 3;

      // Update Back center Display State
      if (Last_Back_Center_State != Back_Center_State){
        updateSensor(REAR_CENTER, OBJECT_VERY_NEAR);
        Last_Back_Center_State = 3; 
      }
    } else if (distance > 35 && distance <= 55){
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep_left(distance, frequency);
      updateBeep_right(distance, frequency);
      Back_Center_State = 2;

      // Update Back center Display State
      if (Last_Back_Center_State != Back_Center_State){
        updateSensor(REAR_CENTER, OBJECT_NEAR);
        Last_Back_Center_State = 2; 
      }
    } else if (distance > 55 && distance <= 100){
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep_left(distance, frequency);
      updateBeep_right(distance, frequency);
      Back_Center_State = 1;

      // Update Back center Display State
      if (Last_Back_Center_State != Back_Center_State){
        updateSensor(REAR_CENTER, OBJECT_FAR);
        Last_Back_Center_State = 1; 
      }
    } else{
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      noTone(SPEAKER_PIN_LEFT);
      noTone(SPEAKER_PIN_RIGHT);
      Back_Center_State = 0;

      // Update Back center Display State
      if (Last_Back_Center_State != Back_Center_State){
        updateSensor(REAR_CENTER, NO_OBJECT);
        Last_Back_Center_State = 0; 
      }
    }
  }
}
void Back_Left(){
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
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep_left(distance, frequency);
      Back_Left_State = 3;

      // Update Back left Display State
      if (Last_Back_Left_State != Back_Left_State){
        updateSensor(REAR_LEFT, OBJECT_VERY_NEAR);
        Last_Back_Left_State = 3; 
      }
    } else if (distance > 35 && distance <= 55){
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep_left(distance, frequency);
      Back_Left_State = 2;

      // Update Back left Display State
      if (Last_Back_Left_State != Back_Left_State){
        updateSensor(REAR_LEFT, OBJECT_NEAR);
        Last_Back_Left_State = 2; 
      }
    } else if (distance > 55 && distance <= 100){
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep_left(distance, frequency);
      Back_Left_State = 1;

      // Update Back left Display State
      if (Last_Back_Left_State != Back_Left_State){
        updateSensor(REAR_LEFT, OBJECT_FAR);
        Last_Back_Left_State = 1; 
      }
    } else{
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      noTone(SPEAKER_PIN_LEFT);
      Back_Left_State = 0;

      // Update Back left Display State
      if (Last_Back_Left_State != Back_Left_State){
        updateSensor(REAR_LEFT, NO_OBJECT);
        Last_Back_Left_State = 0; 
      }
    }
  }
}
void Back_Right(){
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
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep_right(distance, frequency);
      Back_Right_State = 3;

      // Update Back right Display State
      if (Last_Back_Right_State != Back_Right_State){
        updateSensor(REAR_RIGHT, OBJECT_VERY_NEAR);
        Last_Back_Right_State = 3; 
      }
    } else if (distance > 35 && distance <= 55){
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep_right(distance, frequency);
      Back_Right_State = 2;

      // Update Back right Display State
      if (Last_Back_Right_State != Back_Right_State){
        updateSensor(REAR_RIGHT, OBJECT_NEAR);
        Last_Back_Right_State = 2; 
      }
    } else if (distance > 55 && distance <= 100){
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      updateBeep_right(distance, frequency);
      Back_Right_State = 1;

      // Update Back right Display State
      if (Last_Back_Right_State != Back_Right_State){
        updateSensor(REAR_RIGHT, OBJECT_FAR);
        Last_Back_Right_State = 1; 
      }
    } else{
      Serial.print("Entfernung: ");
      Serial.print(distance);
      Serial.println(" cm");
      noTone(SPEAKER_PIN_LEFT);
      Back_Right_State = 0;

      // Update Back right Display State
      if (Last_Back_Right_State != Back_Right_State){
        updateSensor(REAR_RIGHT, NO_OBJECT);
        Last_Back_Right_State = 0; 
      }
    }
  }
}

void loop() {
  Front_Center();
  Front_Left();
  Front_Right();

  Back_Center();
  Back_Left();
  Back_Right();
}


void updateBeep_right(float distance_right, int frequency) {
  unsigned long currentTime = millis();
  unsigned long beepInterval_right;

  // Abstand -> Geschwindigkeit
  if (distance_right <= 35) {
    beepInterval_right = 150;
  }
  else if (distance_right > 35 && distance_right <= 55) {
    beepInterval_right = 400;
  }
  else if (distance_right > 55 && distance_right <= 100) {
    beepInterval_right = 800;
  }
  else {
    noTone(SPEAKER_PIN_RIGHT);
    beepActive_right = false;
    return;
  }

  // Piepton starten
  if (!beepActive_right && currentTime - lastBeep_right >= beepInterval_right) {
    lastBeep_right = currentTime;
    tone(SPEAKER_PIN_RIGHT, frequency);
    beepActive_right = true;
  }

  // Piepton beenden
  if (beepActive_right && currentTime - lastBeep_right >= BEEP_DURATION) {
    noTone(SPEAKER_PIN_RIGHT);
    beepActive_right = false;
  }
}

void updateBeep_left(float distance_left, int frequency) {
  unsigned long currentTime = millis();
  unsigned long beepInterval_left;

  // Abstand -> Geschwindigkeit
  if (distance_left <= 35) {
    beepInterval_left = 150;
  }
  else if (distance_left > 35 && distance_left <= 55) {
    beepInterval_left = 400;
  }
  else if (distance_left > 55 && distance_left <= 100) {
    beepInterval_left = 800;
  }
  else {
    noTone(SPEAKER_PIN_LEFT);
    beepActive_left = false;
    return;
  }

  // Piepton starten
  if (!beepActive_left && currentTime - lastBeep_left >= beepInterval_left) {
    lastBeep_left = currentTime;
    tone(SPEAKER_PIN_LEFT, frequency);
    beepActive_left = true;
  }

  // Piepton beenden
  if (beepActive_left && currentTime - lastBeep_left >= BEEP_DURATION) {
    noTone(SPEAKER_PIN_LEFT);
    beepActive_left = false;
  }
}