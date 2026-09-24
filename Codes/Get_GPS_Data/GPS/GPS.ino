#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// GPS TX -> Arduino D4
// GPS RX -> Arduino D3
SoftwareSerial gpsSerial(4, 3);

TinyGPSPlus gps;

double lastLatitude = 0.0;
double lastLongitude = 0.0;

void setup()
{
  Serial.begin(9600);
  gpsSerial.begin(9600);

  Serial.println("GPS startet...");
}

void loop()
{
  // Alle verfügbaren GPS-Daten einlesen
  while (gpsSerial.available() > 0)
  {
    gps.encode(gpsSerial.read());
  }

  // Sobald eine neue Position verfügbar ist
  if (gps.location.isUpdated())
  {
    lastLatitude = gps.location.lat();
    lastLongitude = gps.location.lng();

    Serial.println("----- GPS Daten -----");

    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);

    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);

    Serial.println();
  }
}
