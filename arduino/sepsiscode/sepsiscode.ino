#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include "MAX30105.h"

MAX30105 particleSensor;

// Wi-Fi credentials
const char* ssid = "iPhone 15 pro";       
const char* password = "123456789ak";

// 🔹 Your Flask server IP (shown in terminal after running Flask)
String serverName = "http://172.20.10.10:5000/data";


// ----------- Sensor Coefficients (example calibration) -----------
#define HR_COEFF_RED 2.6574e-05   
#define HR_COEFF_IR 2.5909e-04    
#define HR_OFFSET 47.42           
#define SPO2_COEFF_RED 6.9844e-07 
#define SPO2_COEFF_IR -6.1744e-06 
#define SPO2_OFFSET 96.63         
#define IR_THRESHOLD 70000        

// ----------- Timers -----------
unsigned long lastTime = 0;
unsigned long timerDelay = 3000;  // every 3 seconds

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing MAX30105 and WiFi...");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("❌ MAX30105 not found. Please check wiring!");
    while (1);
  }

  particleSensor.setup();  // Default configuration
  Serial.println("✅ MAX30105 initialized successfully.");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected!");
  Serial.print("📡 ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    lastTime = millis();

    float red = particleSensor.getRed();
    float ir = particleSensor.getIR();
    float heartRate = -999, spO2 = -999, temperature = -999, respiratoryRate = -999;

    if (ir > IR_THRESHOLD) {
      // Basic calibration equations (customize as per your sensor)
      heartRate = (HR_COEFF_RED * red) + (HR_COEFF_IR * ir) + HR_OFFSET;
      spO2 = (SPO2_COEFF_RED * red) + (SPO2_COEFF_IR * ir) + SPO2_OFFSET;
      temperature = particleSensor.readTemperature();

      // ✅ Calculate respiratory rate automatically
      respiratoryRate = (heartRate / 4.0) - 2;

      Serial.printf("❤️ HR: %.2f bpm | 🩸 SpO2: %.2f %% | 🌡 Temp: %.2f °C | 🌬 RespRate: %.2f\n",
                    heartRate, spO2, temperature, respiratoryRate);

      // Send to Flask
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverName);
        http.addHeader("Content-Type", "application/json");

        // ✅ Send all sensor values including respiratory rate
        String jsonData = "{\"heartRate\": " + String(heartRate, 2) +
                          ", \"spo2\": " + String(spO2, 2) +
                          ", \"temperature\": " + String(temperature, 2) +
                          ", \"respiratoryRate\": " + String(respiratoryRate, 2) + "}";

        int httpResponseCode = http.POST(jsonData);

        if (httpResponseCode > 0) {
          Serial.println("✅ Data sent successfully (" + String(httpResponseCode) + ")");
        } else {
          Serial.println("❌ Error sending data: " + String(httpResponseCode));
        }

        http.end();
      } else {
        Serial.println("⚠️ WiFi not connected!");
      }
    } else {
      Serial.println("👉 Place your finger properly on the sensor.");
    }
  }
}
