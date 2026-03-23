#include <WiFiNINA.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
//light sensor
#include <BH1750.h>
#include <Wire.h>

//dht stuff
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11

//light sensor
BH1750 lightMeter(0x23);

//dht vars
DHT dht(DHTPIN, DHTTYPE);

char ssid[] = SECRET_SSID;    //  your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

float temperature = 0;
float light = 0;

void setup() {
  Serial.begin(115200);  // Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
  //start dht module
  dht.begin();
  //start lights module
  startLightFields();
    
  ThingSpeak.begin(client);  //Initialize ThingSpeak
}

void loop() {
  // Connect or reconnect to WiFi
  connectToWifi();

  //get values
  temperature = getTemp();
  light = getLightL();

  //setfields
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, light);

  //write to fields
  writeToFields();
  
  delay(30000); //wait 30 seconds
}



void connectToWifi() {
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
}

void writeToFields() {
  int statusCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  //check for issues
  if(statusCode == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(statusCode));
  }
}

