#include <WiFiNINA.h>
#include "secrets.h"

//light sensor
#include <BH1750.h>
#include <Wire.h>

char ssid[] = SECRET_SSID;         
char pass[] = SECRET_PASS; 

//light sensor
BH1750 lightMeter(0x23);

WiFiClient client;

float light = 0;

bool sunStatus = false;

float luxMin = 5000; //change based on where placed, as 5000 is flashlight up close level

//unsigned long EMAIL_INTERVAL = 60000;

char HOST_NAME[] = "maker.ifttt.com";
String PATH_NAME   = "/trigger/get_light/with/key/-64nNTNE4gnCkWZMvqTGE"; // change your EVENT-NAME and YOUR-KEY
String queryString = "?value1=57&value2=25";

void setup() {
  WiFi.begin(ssid, pass);

  Serial.begin(9600);
  while (!Serial);

  // connect to web server on port 80:
  if (client.connect(HOST_NAME, 80)) {
    // if connected:
    Serial.println("Connected to server");
  }
  else {// if not connected:
    Serial.println("connection failed");
  }

  startLight();

}

void loop() {
  light = getLightL();
  checkLightLevel();
  
  delay(10000);

}

void checkLightLevel() {
  if (light >= 1) {
    bool isSunny = light >= luxMin;

    if (isSunny != sunStatus) {
      sunStatus = isSunny;

      switch (sunStatus) {
        case true:  //sun
          sendEmail(true);
          break;
        case false: //shade
          sendEmail(false);
          break;
      }
    }
  }
}

void sendEmail(bool sunny) {
  String value = "Dark";
  if(sunny) {
    value = "Sunny";
  }

  String queryString = "?value1=" + value;

  if (client.connect(HOST_NAME, 80)) { //bool
    Serial.println("Connected to IFTTT");
    client.println("GET " + PATH_NAME + queryString + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println();

    while (client.connected()) {
      while (client.available()) {
        char c = client.read();
        Serial.print(c);
      }
    }

    client.stop();
    Serial.println();
    Serial.println("Disconnected from IFTTT");
  } else {
    Serial.println("Connection to IFTTT failed");
  }
}

//based on setup
void startLight() {
  Wire.begin();
  // begin returns a boolean that can be used to detect setup problems.
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  } else {
    Serial.println(F("Error initialising BH1750"));
  }
}

float getLightL() {
  if (lightMeter.measurementReady()) {
    float lux = lightMeter.readLightLevel();
    return lux;
  }
  return -1;
}
