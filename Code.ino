#include <TFT_eSPI.h>
#include <Arduino.h>
#include <time.h>
#include <stdio.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

TFT_eSPI tft = TFT_eSPI();
#define BOOT_BUTTON_PIN 14

const char *ssid = "";
const char *password = "";
const char *mqtt_broker = "schulsani.local";
const char *topic = "";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;
const int buzzerPin =  16;
const int batteryPin = 4; // Battery voltage measurement pin
const float voltageDividerRatio = 2.0; // Voltage divider ratio
const float maximumVoltage = 3.7; // Maximum voltage of the battery
int standartbatteryPercentage; // Global variable declaration
int alarmbest= 0;

WiFiClient espClient;
PubSubClient client(espClient);

int alarmState = 0;
const char* unfall = "";  // Initialize as an empty string
const char* raum = "";    // Initialize as an empty string
const char* unfall_1 = "";  // Initialize as an empty string
const char* raum_1 = "";  


void callback(char* topic, byte* payload, unsigned int length) {
    if (strcmp(topic, "send") == 0) {
        Serial.print("Message arrived in topic: ");
        Serial.println(topic);

        Serial.println("Received payload:");
        for (int i = 0; i < length; i++) {
            Serial.print((char)payload[i]);
        }
        Serial.println();
       
        DynamicJsonDocument doc(2 * JSON_OBJECT_SIZE(3));
        deserializeJson(doc, (char*)payload);
        const char* receivedAlarm = doc["alarm"]; 
        alarmState = atoi(receivedAlarm);
        if (alarmState == 2) {
          alarmbest= 0;
        }
        unfall = doc["unfall"];
        raum = doc["raum"];
        Serial.println(alarmbest);
        if (alarmState >= 0 ) {
          if (alarmbest == 0) {
            tft.fillScreen(TFT_RED);
            Serial.println("1");
          }
          else {
            tft.fillScreen(TFT_BLACK);
            Serial.println("2");
          }
        }
        
        displayText(25, 10, 2, "Schulsani Pager");
        Serial.println("Unfall: " + String(unfall));
        Serial.println("Raum: " + String(raum));
        Serial.println("Alarm: " + String(receivedAlarm));
        tft.setTextSize(2.5);
        tft.setCursor(25, 50);
        tft.print("Unfall: " + String(unfall));
        tft.setTextSize(2.5);
        tft.setCursor(25, 90);
        tft.print("Raum: " + String(raum));
        delay(2000);
        Serial.println();
        Serial.println("-----------------------");
    }
}

void playMelody() {
  // Define the notes and their corresponding durations
  int melody[] = {262, 294, 330, 349, 392, 440, 494, 523};
  int noteDurations[] = {4, 4, 4, 4, 4, 4, 4, 4}; // in beats

  // Iterate through the melody
  for (int i = 0; i < 8; i++) {
    int noteDuration = 1000 / noteDurations[i];
    tone(buzzerPin, melody[i], noteDuration);

    // Pause between notes
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    // Stop the tone to create a distinct sound between each note
    noTone(buzzerPin);
  }
}
void batteryprozent() {
  int rawValue = analogRead(batteryPin); // Read raw ADC value
  float voltage = (rawValue / 4095.0) * maximumVoltage * voltageDividerRatio; // Calculate voltage
  int batteryPercentage = map(voltage, 3.0, 4.2, 0, 100); // Convert voltage to percentage
  tft.setCursor(0, 0); // Clear the screen
  tft.setTextColor(TFT_WHITE);
  Serial.print(batteryPercentage);
  if (batteryPercentage = standartbatteryPercentage) {
    Serial.print("");
  }
  else {
    if (batteryPercentage <= 101) {
      Serial.print("Battery Level: ");
      Serial.print(batteryPercentage);
      Serial.println("%");
      tft.setTextSize(2); // Set text size
      tft.println(batteryPercentage + "%");
      int standartbatteryPercentage = batteryPercentage;
    }
    else {
      tft.println("Batterie nicht verbunden");
      Serial.print("Batterie nicht verbunden");
      Serial.print(batteryPercentage);
    }
  }
}
void setup() {
    pinMode(15, OUTPUT);
    digitalWrite(15, HIGH);
    Serial.begin(115200);
    tft.fillScreen(TFT_BLACK);
    delay(1000);
    int rawValue = analogRead(batteryPin); // Read raw ADC value
    float voltage = (rawValue / 4095.0) * maximumVoltage * voltageDividerRatio; // Calculate voltage
    standartbatteryPercentage = map(voltage, 3.0, 4.2, 0, 100); // Convert voltage to percentage
    pinMode(buzzerPin, OUTPUT);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");
    tft.setTextSize(5);
    tft.print("\nConnecting");
    Serial.println("\nConnecting");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nWlan wurde verbunden");
    tft.print("Wlan wurde verbunden");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());

    tft.init();
    tft.setRotation(1);

    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);

    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);

    int attempt = 0;
    while (!client.connected()) {
        String client_id = "esp32-client-" + String(WiFi.macAddress());
        Serial.printf("Attempting connection, Client ID: %s, Attempt: %d\n", client_id.c_str(), attempt);

        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Connected to MQTT broker");
            tft.print("Verbindung wurde zum MQTT Broker verbunden");
        } else {
            Serial.print("Failed to connect to MQTT broker. State: ");
            tft.print("Verbindung zum MQTT Broker ist fehlgeschlagen");
            Serial.print(client.state());
            Serial.println(". Retrying...");
            delay(2000);
        }
        attempt++;
    }
    tft.fillScreen(TFT_BLACK);
    client.subscribe("send");
}

void loop() {
    displayText(25, 10, 2, "Schulsani Pager");
    if (alarmState == 2) {
        playMelody();
    }

    client.loop();
    if (digitalRead(BOOT_BUTTON_PIN) == LOW) {
        raum_1 = raum;
        unfall_1 = unfall;
        
        // Declare the message variable
        String message = "{\"raum\":\"" + String(raum_1) + "\",\"unfall\":\"" + String(unfall_1) + "\",\"alarm\":\"" + "1" + "\"}";


        client.publish("send", message.c_str());
        if (alarmState == 2) {
          client.publish("alarm", "Alarm wurde von einem Schulsanitätern bestätigt");
        }
        else {
          client.publish("alarm", "Alarm wurde von mehreren Schulsanitätern bestätigt");
        }
        alarmbest = 1;
        

    }
    // batteryprozent();
}


void displayText(int x, int y, int size, const char *text) {
    tft.setCursor(x, y);
    tft.setTextSize(size);
    tft.print(text);
}