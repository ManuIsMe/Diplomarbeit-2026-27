const byte TRIG_PIN = 9;
const byte ECHO_PIN = 10;

void setup()
{
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(TRIG_PIN, LOW);

  delay(1000);
}

void loop()
{
  // Trigger-Puls
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(20);
  digitalWrite(TRIG_PIN, LOW);

  // Echo abwarten
  unsigned long dauer = pulseIn(ECHO_PIN, HIGH, 60000);

  Serial.print("Echo: ");
  Serial.print(dauer);
  Serial.println(" us");

  if (dauer > 0)
  {
    float entfernung = dauer * 0.0343 / 2.0;

    Serial.print("Entfernung: ");
    Serial.print(entfernung);
    Serial.println(" cm");
  }
  else
  {
    Serial.println("KEIN ECHO");
  }

  Serial.println("----------------");

  // etwas länger warten
  delay(1000);
}