#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include "MAX30105.h"
#include "filters.h"

MAX30105 sensor;
WiFiClient client;

const char* ssid = "your_ssid";
const char* password = "your_password";
unsigned long myChannelNumber = YOUR_CHANNEL_ID;
const char* myWriteAPIKey = "YOUR_API_KEY";

float kSpO2_A = 1.5958422;
float kSpO2_B = -34.6596622;
float kSpO2_C = 112.6898759;
float temp, heartRate, spo2;
int ecgPin = A0;  // Pin connected to ECG sensor output
float ecgValue;

void setup() {
  Serial.begin(9600);

  // WiFi Connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  ThingSpeak.begin(client);

  // MAX30105 Initialization
  if (!sensor.begin(Wire)) {
    Serial.println("MAX30105 not found.");
    while (1);
  }
  sensor.setup();
  sensor.enableDIETEMPRDY();
}

void loop() {
  if (sensor.begin()) {
    readHealthParameters();
    sendToThingSpeak();
  }
  delay(20000);
}

void readHealthParameters() {
  temp = sensor.readTemperature();

  float current_value_red = sensor.getRed();
  float current_value_ir = sensor.getIR();

  heartRate = calculateHeartRate(current_value_red);
  spo2 = calculateSpO2(current_value_red, current_value_ir);
  ecgValue = readECG();

  Serial.print("Temperature: ");
  Serial.println(temp);
  Serial.print("Heart Rate: ");
  Serial.println(heartRate);
  Serial.print("SpO2: ");
  Serial.println(spo2);
  Serial.print("ECG Value: ");
  Serial.println(ecgValue);
}

float readECG() {
  int rawEcg = analogRead(ecgPin);
  return (float)rawEcg * (3.3 / 1023.0); // Scale to voltage (3.3V system)
}

void sendToThingSpeak() {
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, heartRate);
  ThingSpeak.setField(3, spo2);
  ThingSpeak.setField(4, ecgValue);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Data successfully sent to ThingSpeak");
  } else {
    Serial.println("Error sending data");
  }
}

float calculateHeartRate(float redValue) {
  return random(60, 100); // Placeholder value
}

float calculateSpO2(float redValue, float irValue) {
  float r = redValue / irValue;
  float spo2 = kSpO2_A * r * r + kSpO2_B * r + kSpO2_C;
  return spo2;
}
