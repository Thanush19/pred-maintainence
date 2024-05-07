#include "ACS712.h"

// Vibration Sensor - SW420
const int vibrationSensorPin = A0; // GPIO36 - ADC1_0

// Speed Sensor
const int speedSensorPin = 23; // GPIO
volatile unsigned long pulseCount = 0;
unsigned long lastTime = 0;
unsigned long rpm = 0;

// Sound Sensor - LM393
const int soundSensorPin = A3; // ADC1_3

// Temperature Sensor - LM35
const int temperatureSensorPin = A6; // ADC1_6

// Voltage Sensor - ZMPT101B
const int voltageSensorPin = A7; // ADC1_7

// Current Sensor - ACS712
ACS712 currentSensor(A5, 3.3, 4095, 185); // Change pins and parameters if necessary

void setup() {
  Serial.begin(115200);

  // Speed Sensor setup
  pinMode(speedSensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(speedSensorPin), countPulses, FALLING);
}

void loop() {
  printSensorValues();
  delay(1000);
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

void printSensorValues() {
  // Vibration Sensor - SW420
  int vibrationValue = analogRead(vibrationSensorPin);
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
  int db = map(peakToPeak, 20, 900, 49.5, 90);
  Serial.print("Loudness: ");
  Serial.print(db);
  Serial.println("dB");

  // Temperature Sensor - LM35
  int reading = analogRead(temperatureSensorPin);
  float voltage = reading * (5.0 / 1024.0);
  float temperatureC = voltage * 100;
  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.print("\xC2\xB0C  |  ");
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  Serial.print(temperatureF);
  Serial.print("\xC2\xB0F\n");

  // Voltage Sensor - ZMPT101B
  int voltageValue = analogRead(voltageSensorPin);
  Serial.print("Voltage: ");
  Serial.println(voltageValue);

  // Current Sensor - ACS712
  int currentmA = currentSensor.mA_AC_sampling();
  Serial.print("Current: ");
  Serial.print(currentmA);
  Serial.println(" mA");
}