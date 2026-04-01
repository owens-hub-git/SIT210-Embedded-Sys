//light sensor
#include <BH1750.h>
#include <Wire.h>

BH1750 lightMeter(0x23);

const int pirPin = 2;
const int ledPin1 = 8;
const int ledPin2 = 10;

const int switchPin = 3;

bool isDark = false;

volatile int switchState = LOW;

volatile int pirMotion = LOW;

float light = 0;
float luxForDark = 300;

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("STARTING...");
  startLight();

  attachInterrupt(digitalPinToInterrupt(switchPin), toggleSwitch, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pirPin), motionDetected, CHANGE);
}

void loop() {
  isDark = isDarkFunc();
}

void motionDetected() {
  if (isDark && switchState == LOW) {
    pirMotion = !pirMotion;
    Serial.println("Motion Detected!");
    digitalWrite(ledPin1, pirMotion);
    digitalWrite(ledPin2, pirMotion);
    Serial.print("Lights: ");
    if (pirMotion == HIGH) {
      Serial.println("ON");
    }
    else {
      Serial.println("OFF");
    }
  }
  else {
    Serial.println("Not Dark Enough Yet or The Switch is ON");
  }
}

void toggleSwitch() {
  switchState = digitalRead(switchPin);

  Serial.println("Light Switched!");
  digitalWrite(ledPin1, switchState);
  digitalWrite(ledPin2, switchState);
  Serial.println("Lights: ");
  if (switchState == LOW) {
    Serial.println("OFF");
  }
  else {
    Serial.println("ON");
  }

}

void startLight() {
  
  Wire.begin();
  // begin returns a boolean that can be used to detect setup problems.
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  } else {
    Serial.println(F("Error initialising BH1750"));
  }
}

void getLightLevel() {
  if (lightMeter.measurementReady()) {
    light = lightMeter.readLightLevel();
  }
}

bool isDarkFunc() {
  getLightLevel();
  if (light <= luxForDark) {
    //Serial.println(light);
    return true;
  }
  return false;
}
