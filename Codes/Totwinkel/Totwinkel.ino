
const byte TRIG1 = 9;
const byte ECHO1 = 10;

const byte TRIG2 = 7;
const byte ECHO2 = 8;

const byte SPEAKER_PIN_1 = 11;
const byte SPEAKER_PIN_2 = 12;

enum Zustand {
  TRIGGER_START,
  TRIGGER_ENDE,
  ECHO_WARTEN,
  ECHO_MESSEN,
  MESSPAUSE
};

Zustand zustand = TRIGGER_START;

byte sensor = 1;

unsigned long zeitStart = 0;
unsigned long echoStart = 0;
unsigned long letzteAusgabe = 0;

float entfernung1 = 0;
float entfernung2 = 0;

bool echoFehler = false;

int frequency = 500;
unsigned long lastBeep = 0;
bool beepActive1 = false;
bool beepActive2 = false;
const unsigned long BEEP_DURATION = 50;
unsigned long currentTime = millis();
unsigned long beepInterval = 400;

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

  pinMode(LAUTSPRECHER1, OUTPUT);
  pinMode(LAUTSPRECHER2, OUTPUT);

  digitalWrite(TRIG1, LOW);
  digitalWrite(TRIG2, LOW);

  digitalWrite(LAUTSPRECHER1, LOW);
  digitalWrite(LAUTSPRECHER2, LOW);
}

void loop() {
  unsigned long jetzt = micros();

  switch (zustand) {

    case TRIGGER_START:
      digitalWrite(trigPin(), LOW);

      if (jetzt - zeitStart >= 2) {
        digitalWrite(trigPin(), HIGH);
        zeitStart = micros();
        zustand = TRIGGER_ENDE;
      }
      break;

    case TRIGGER_ENDE:
      if (jetzt - zeitStart >= 10) {
        digitalWrite(trigPin(), LOW);
        zeitStart = micros();
        zustand = ECHO_WARTEN;
      }
      break;

    case ECHO_WARTEN:
      if (digitalRead(echoPin()) == HIGH) {
        echoStart = micros();
        zustand = ECHO_MESSEN;
      } else if (jetzt - zeitStart >= 30000) {
        if (sensor == 1) {
          entfernung1 = 0;
        } else {
          entfernung2 = 0;
        }

        zustand = MESSPAUSE;
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

        zustand = MESSPAUSE;
        zeitStart = micros();
      } else if (jetzt - echoStart >= 30000) {
        if (sensor == 1) {
          entfernung1 = 0;
        } else {
          entfernung2 = 0;
        }

        zustand = MESSPAUSE;
        zeitStart = micros();
      }
      break;

    case MESSPAUSE:
      if (jetzt - zeitStart >= 50000) {
        if (sensor == 1) {
          sensor = 2;
        } else {
          sensor = 1;

          if (millis() - letzteAusgabe >= 100) {
            letzteAusgabe = millis();

            Serial.print("Sensor 1: ");
            Serial.print(entfernung1);
            Serial.print(" cm | Sensor 2: ");
            Serial.print(entfernung2);
            Serial.println(" cm");
          }

          if(entfernung1 <= 100){
            beepActive1 = true;
          }
          
          if(entfernung2 <= 100){
            beepActive2 = true;
          }


          // Piepton starten Speaker 1
          if (!beepActive1 &&
            currentTime - lastBeep >= beepInterval) {
              lastBeep = currentTime;
              tone(SPEAKER_PIN_1, frequency);
              beepActive = true;
          }

          // Piepton beenden Speaker 1
          if (beepActive1 &&
          currentTime - lastBeep >= BEEP_DURATION) {
            noTone(SPEAKER_PIN_1);
            beepActive = false;
          }

          // Piepton starten Speaker 2
          if (!beepActive2 &&
            currentTime - lastBeep >= beepInterval) {
              lastBeep = currentTime;
              tone(SPEAKER_PIN_1, frequency);
              beepActive = true;
          }

          // Piepton beenden Speaker 2
          if (beepActive2 &&
          currentTime - lastBeep >= BEEP_DURATION) {
            noTone(SPEAKER_PIN_1);
            beepActive = false;
          }
        }

        zeitStart = micros();
        zustand = TRIGGER_START;
      }
      break;
  }
}
