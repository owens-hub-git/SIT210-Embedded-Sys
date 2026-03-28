
//based on setup
void startLightFields() {
  pinMode(lightPin, INPUT);
}

int getLightL() {
  lightValue = analogRead(lightPin);
  Serial.print("light = ");
  Serial.println(lightValue);
  return lightValue;
}
