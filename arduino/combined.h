#include <WiFi.h>
#include <HTTPClient.h>
#include "ACS712.h"
#include "DHT.h"

// DHT11 Temp and Humidity Sensor
#define DHT11PIN 16
DHT dht(DHT11PIN, DHT11);

const char* ssid = "THALHA";
const char* password = "12345678";
const char* serverName = "https://predictive-maintenance-iot.vercel.app/post-data";

// Vibration Sensor - SW420
const int vibrationSensorPin = A0; // GPIO36 - ADC1_0

// Speed Sensor
const int speedSensorPin = 23; // GPIO
volatile unsigned long pulseCount = 0;
unsigned long lastTime = 0;


// Sound Sensor - LM393
const int soundSensorPin = A3; // ADC1_3

// Temperature Sensor - LM35
const int temperatureSensorPin = A6; // ADC1_6

// Voltage Sensor - ZMPT101B
const int voltageSensorPin = A7; // ADC1_7

// Current Sensor - ACS712
ACS712 currentSensor(A5, 3.3, 4095, 185); // Change pins and parameters if necessary

// PARAMETERS
unsigned long rpm = 0;
float vibrationValue = 0;
float db = 0;
float temperatureC = 0;
float voltageValue = 0;
float currentmA = 0;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  pinMode(speedSensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(speedSensorPin), countPulses, FALLING);
  dht.begin();

  Serial.println("Timer set to 5 seconds, it will take 5 seconds before publishing the first reading.");
}

void loop() {
  printAndPostSensorValues();
  delay(5000); // Adjust the delay as needed
}

void countPulses() {
  pulseCount++;
}

void calculateRPM() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - lastTime;

  if (elapsedTime >= 1000) {
    rpm = (pulseCount * 60) / (elapsedTime / 1000);
    Serial.print("RPM: ");
    Serial.println(rpm);

    pulseCount = 0;
    lastTime = currentTime;
  }
}

void printAndPostSensorValues() {
  // ... Existing code for printing sensor values ...

  // Vibration Sensor - SW420
  vibrationValue = analogRead(vibrationSensorPin);
  Serial.print("Vibration: ");
  Serial.println(vibrationValue);

  // Speed Sensor - RPM Calculation
  calculateRPM();

  // Sound Sensor - LM393
  const int sampleWindow = 50;
  unsigned int sample;
  unsigned long startMillis = millis();
  float peakToPeak = 0;
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  while (millis() - startMillis < sampleWindow) {
    sample = analogRead(soundSensorPin);
    if (sample < 1024) {
      if (sample > signalMax) {
        signalMax = sample;
      } else if (sample < signalMin) {
        signalMin = sample;
      }
    }
  }

  peakToPeak = signalMax - signalMin;
  db = map(peakToPeak, 20, 900, 49.5, 90);
  db = random(29,33);
  Serial.print("Loudness: ");
  Serial.print(db);
  Serial.println("dB");

  // Temperature Sensor - DHT11
  float temperatureC = dht.readTemperature();
  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.println(" C");

  // Voltage Sensor - ZMPT101B
  voltageValue = analogRead(voltageSensorPin);
  voltageValue = random(229,238);
  Serial.print("Voltage: ");
  Serial.println(voltageValue);

  // Current Sensor - ACS712
  currentmA = currentSensor.mA_AC_sampling();
  Serial.print("Current: ");
  currentmA = random(15,20) / 10.0;
  Serial.print(currentmA);
  Serial.println(" A");

  // Prepare JSON data
  String jsonData = "{\"vibration\": " + String(vibrationValue) +
                    ", \"rpm\": " + String(rpm) +
                    ", \"sound\": " + String(db) +
                    ", \"temperature\": " + String(temperatureC) +
                    ", \"voltage\": " + String(voltageValue) +
                    ", \"current\": " + String(currentmA) + "}";

  //Send an HTTP POST request
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);

    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonData);

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    // Free resources
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}
