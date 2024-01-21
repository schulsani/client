#include <TFT_eSPI.h>
#include <Arduino.h>
#include <time.h>
#include <stdio.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

TFT_eSPI tft = TFT_eSPI();
#define BOOT_BUTTON_PIN 0

const char *ssid = "";
const char *password = "";
const char *ntpServerName = "pool.ntp.org";
const char *mqtt_broker = "schulsani.local";
const char *topic = "";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;
const int buzzerPin =  1;


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
        
        uint16_t red = TFT_RED;
        tft.fillRect(0, 0, tft.width(), tft.height(), red);
        unfall = doc["unfall"];
        raum = doc["raum"];
        displayText(25, 10, 2, "Schulsani Pagger");
        const char* receivedAlarm = doc["alarm"];
        Serial.println("Unfall: " + String(unfall));
        Serial.println("Raum: " + String(raum));
        Serial.println("Alarm: " + String(receivedAlarm));

        tft.setTextSize(2.5);
        tft.setCursor(25, 50);
        tft.print("Unfall: " + String(unfall));
        tft.setTextSize(2.5);
        tft.setCursor(25, 90);
        tft.print("Raum: " + String(raum));
        unfall_1 = unfall;
        raum_1 = raum;
        delay(2000);

        Serial.println();
        Serial.println("-----------------------");
        alarmState = atoi(receivedAlarm);
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
void setup() {

    Serial.begin(115200);
    tft.fillScreen(TFT_BLACK);
    delay(1000);
    pinMode(buzzerPin, OUTPUT);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nConnected to the WiFi network");
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
        } else {
            Serial.print("Failed to connect to MQTT broker. State: ");
            Serial.print(client.state());
            Serial.println(". Retrying...");
            delay(2000);
        }
        attempt++;
    }

    client.subscribe("send");
}

void loop() {
    displayText(25, 10, 2, "Schulsani Pagger");
    tft.setTextSize(2.5);
    tft.setCursor(25, 50);
    tft.print("Unfall: " + String(unfall));
    tft.setTextSize(2.5);
    tft.setCursor(25, 90);
    tft.print("Raum: " + String(raum));
    delay(2000);

    if (digitalRead(BOOT_BUTTON_PIN) == LOW) {
        alarmState = 0;
        client.publish("alarm", "Alarm wurde von den Schulsanitätern bestätigt");
        tft.fillScreen(TFT_BLACK);
        delay(1000);
    }
    if (alarmState == 1) {
        playMelody();
    }
    else {
    }
    client.loop();
}

void displayText(int x, int y, int size, const char *text) {
    tft.setCursor(x, y);
    tft.setTextSize(size);
    tft.print(text);
}
