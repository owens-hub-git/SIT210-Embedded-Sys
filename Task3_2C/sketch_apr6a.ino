#include <ArduinoMqttClient.h>
#include <NewPing.h>
#include <Arduino.h>
#include <Vector.h>
#include <WiFiNINA.h>
#include "secrets.h"

#define TRIGGER_PIN  9  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     10  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 150 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

//wifi stuff
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "q2a11182.ala.dedicated.aws.emqxcloud.com";
int        port     = 1883;
const char wave_topic[]  = "/wave_topic";
const char pat_topic[]  = "/pat_topic";
const char username[] = "arduinouser";
const char password[] = "password";

//.push_back() is essentially append()
//.remove() for removing first element
//.assign() for start with vals

const int ELEMENT_COUNT_MAX = 10; //for every 200ms to 2 seconds = 10
typedef Vector<int> Elements;
int first_element = 0;
int storage_array[ELEMENT_COUNT_MAX];
Elements vector;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
unsigned int sonar_distance = 0;

unsigned long lastGestureTime = 0;
const int GESTURE_COOLDOWN = 10000; //10 sec

const int bathroom = 3;
const int hallway = 5;

bool lightsOn = false;

int count = 0;

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  while (!Serial) {
    ;
  }
  pinMode(bathroom, OUTPUT);
  pinMode(hallway, OUTPUT);
  createVector(); //make actual vector
  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  mqttClient.setId("owenarduinomqttaccount");

  mqttClient.setUsernamePassword(username, password);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void loop() {
  mqttClient.poll();

  pingSonar();
  popAndPush();
  //printVector(vector);
  unsigned long currentTime = millis();

  if (currentTime - lastGestureTime > GESTURE_COOLDOWN) {
    int wave = handWave();
    int pat  = handPat();

    if (wave == 1) {
      count++;
      Serial.println("Wave detected!");
      lightsOn = !lightsOn;
      updateLights();
      lastGestureTime = millis();

      //MQTT publish
      mqttClient.beginMessage(wave_topic);
      mqttClient.println("wave");
      mqttClient.print("Lights: ");
      mqttClient.println(lightsOn ? "on" : "off");
      mqttClient.println(count);
      mqttClient.endMessage();
      Serial.println();
    }
    else if (pat == 1) {
      count++;
      Serial.println("PAT detected!");
      lightsOn = !lightsOn;

      updateLights();
      lastGestureTime = millis();

      //MQTT publish
      mqttClient.beginMessage(pat_topic);
      mqttClient.println("pat");
      mqttClient.print("Lights: ");
      mqttClient.println(lightsOn ? "on" : "off");
      mqttClient.println(count);
      mqttClient.endMessage();
      Serial.println();
    }
  }

  delay(200);
}

int handWave() {
  int transitions = 0;

  int lastState = 0;
  if (vector[0] > 5) {
    lastState = 1;
  }

  for (int i = 1; i < vector.size(); i++) {
    int currentState = 0;
    if (vector[i] > 5) {
      currentState = 1;
    }

    if (currentState != lastState) {
      transitions++;
    }

    lastState = currentState;
  }

  if (transitions >= 5) { 
    return 1;
  }
  return 0;
}

int handPat() {
  int size = vector.size();
  if (size < 3) {
    return 0;
  }

  int middle = size / 2;

  int peak = vector[middle];
  if (peak == 0) return 0;

  //left
  for (int i = 0; i < middle - 1; i++) {
    if (vector[i] > vector[i + 1]) {
      return 0;
    }
  }

  //right
  for (int i = middle; i < size - 1; i++) {
    if (vector[i] < vector[i + 1]) {
      return 0;
    }
  }

  return 1;
}

void updateLights() {
  digitalWrite(bathroom, lightsOn ? HIGH : LOW);
  digitalWrite(hallway, lightsOn ? HIGH : LOW);
}

void popAndPush() {
  vector.remove(first_element);
  vector.push_back(sonar_distance);
  //printVector(vector);
}

void pingSonar() {
  //Serial.print("Ping: ");
  sonar_distance = sonar.ping_cm();
  //Serial.print(sonar_distance); // Send ping, get distance in cm and print result (0 = outside set distance range)
  //Serial.println("cm");
}

void createVector() {
  vector.setStorage(storage_array);
  vector.assign(ELEMENT_COUNT_MAX, 1);
  printVector(vector);
}

//use for testing
void printVector(const Elements &vector) {
  Serial.print("[");
  for (size_t i = 0; i < vector.size(); ++i) {
    if (i != 0) {
      Serial.print(",");
    }
    Serial.print(vector[i]);
  }
  Serial.println("]");
}