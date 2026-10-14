const byte TRIG1 = 9;
const byte ECHO1 = 12;

const byte TRIG2 = 7;
const byte ECHO2 = 8;

const byte SPEAKER_PIN_1 = 11;
const byte SPEAKER_PIN_2 = 10;

const unsigned long MESSPAUSE = 50000;
const unsigned long TRIGGER_DAUER = 10;
const unsigned long ECHO_TIMEOUT = 30000;
const unsigned long BEEP_INTERVAL = 400;
const unsigned long BEEP_DURATION = 50;

const int FREQUENCY = 500;

enum Zustand {
  TRIGGER_START,
  TRIGGER_ENDE,
  ECHO_WARTEN,
  ECHO_MESSEN,
  MESSPAUSE_ZUSTAND
};

Zustand zustand = TRIGGER_START;

byte sensor = 1;

unsigned long zeitStart = 0;
unsigned long echoStart = 0;
unsigned long letzteAusgabe = 0;
unsigned long letzteMesspause = 0;
unsigned long letzterBeep = 0;
unsigned long beepStart = 0;

float entfernung1 = 0;
float entfernung2 = 0;

bool alarm1 = false;
bool alarm2 = false;
bool beepAktiv = false;

byte trigPin() {
  return sensor == 1 ? TRIG1 : TRIG2;
}

byte echoPin() {
  return sensor == 1 ? ECHO1 : ECHO2;
}

void setup() {
  Serial.begin(9600);

  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);

  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  pinMode(SPEAKER_PIN_1, OUTPUT);
  pinMode(SPEAKER_PIN_2, OUTPUT);

  digitalWrite(TRIG1, LOW);
  digitalWrite(TRIG2, LOW);

  digitalWrite(SPEAKER_PIN_1, LOW);
  digitalWrite(SPEAKER_PIN_2, LOW);
}

void loop() {
  unsigned long jetztMicros = micros();
  unsigned long jetztMillis = millis();

  switch (zustand) {

    case TRIGGER_START:
      digitalWrite(trigPin(), LOW);

      if (jetztMicros - zeitStart >= 2) {
        digitalWrite(trigPin(), HIGH);
        zeitStart = micros();
        zustand = TRIGGER_ENDE;
      }
      break;

    case TRIGGER_ENDE:
      if (jetztMicros - zeitStart >= TRIGGER_DAUER) {
        digitalWrite(trigPin(), LOW);
        zeitStart = micros();
        zustand = ECHO_WARTEN;
      }
      break;

    case ECHO_WARTEN:
      if (digitalRead(echoPin()) == HIGH) {
        echoStart = micros();
        zustand = ECHO_MESSEN;
      } else if (jetztMicros - zeitStart >= ECHO_TIMEOUT) {
        if (sensor == 1) {
          entfernung1 = 0;
        } else {
          entfernung2 = 0;
        }

        zustand = MESSPAUSE_ZUSTAND;
        zeitStart = micros();
      }
      break;

    case ECHO_MESSEN:
      if (digitalRead(echoPin()) == LOW) {
        unsigned long dauer = micros() - echoStart;
        float entfernung = dauer * 0.0343 / 2.0;

        if (sensor == 1) {
          entfernung1 = entfernung;
        } else {
          entfernung2 = entfernung;
        }

        zustand = MESSPAUSE_ZUSTAND;
        zeitStart = micros();
      } else if (jetztMicros - echoStart >= ECHO_TIMEOUT) {
        if (sensor == 1) {
          entfernung1 = 0;
        } else {
          entfernung2 = 0;
        }

        zustand = MESSPAUSE_ZUSTAND;
        zeitStart = micros();
      }
      break;

    case MESSPAUSE_ZUSTAND:
      if (jetztMicros - zeitStart >= MESSPAUSE) {
        if (sensor == 1) {
          sensor = 2;
        } else {
          sensor = 1;

          if (jetztMillis - letzteAusgabe >= 100) {
            letzteAusgabe = jetztMillis;

            Serial.print("Sensor 1: ");

            if (entfernung1 > 0) {
              Serial.print(entfernung1);
              Serial.print(" cm");
            } else {
              Serial.print("Kein Echo");
            }

            Serial.print(" | Sensor 2: ");

            if (entfernung2 > 0) {
              Serial.print(entfernung2);
              Serial.println(" cm");
            } else {
              Serial.println("Kein Echo");
            }
          }
        }

        zeitStart = micros();
        zustand = TRIGGER_START;
      }
      break;
  }

  alarm1 = entfernung1 > 0 && entfernung1 < 100;
  alarm2 = entfernung2 > 0 && entfernung2 < 100;

  bool alarm = alarm1 || alarm2;

  if (alarm) {
    if (!beepAktiv && jetztMillis - letzterBeep >= BEEP_INTERVAL) {
      letzterBeep = jetztMillis;
      beepStart = jetztMillis;
      tone(SPEAKER_PIN_1, FREQUENCY);
      digitalWrite(SPEAKER_PIN_2, HIGH);
      beepAktiv = true;
    }

    if (beepAktiv && jetztMillis - beepStart >= BEEP_DURATION) {
      noTone(SPEAKER_PIN_1);
      digitalWrite(SPEAKER_PIN_2, LOW);
      beepAktiv = false;
    }
  } else {
    noTone(SPEAKER_PIN_1);
    digitalWrite(SPEAKER_PIN_2, LOW);
    beepAktiv = false;
  }
}