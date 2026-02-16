#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <OneWire.h>
#include <DallasTemperature.h>

MAX30105 particleSensor;
#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing sensors...");

  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("MAX30102 not found. Check wiring!");
    while (1);
  }

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeIR(0x0A);

  sensors.begin();
  Serial.println("Setup complete.\n");
}

void loop() {
  long irValue = particleSensor.getIR();

  if (irValue < 50000) {
    Serial.println("No finger detected...");
  } else {
    if (checkForBeat(irValue) == true) {
      static long lastBeat = 0;
      long delta = millis() - lastBeat;
      lastBeat = millis();

      float beatsPerMinute = 60 / (delta / 1000.0);
      Serial.print("Heart Rate: ");
      Serial.println(beatsPerMinute);
    }

    // Approx SpO2 (simplified)
    long redValue = particleSensor.getRed();
    float ratio = (float)redValue / (float)irValue;
    float spo2 = 110 - 25 * ratio; // basic approximation
    if (spo2 > 100) spo2 = 100;
    if (spo2 < 80) spo2 = 80;

    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0);

    Serial.print("SpO2: ");
    Serial.println(spo2);
    Serial.print("Temperature: ");
    Serial.println(tempC);
  }

  Serial.println("----------------------");
  delay(2000);
}
