#include <DHT.h>

#define DHTPIN A0
#define DHTTYPE DHT11
#define fanButtonPin A5
#define stateButtonPin A1

const int redLEDPin = 2;
const int greenLEDPin = 3;
const int yellowLEDPin = 4;
const int fanPin = 5;


DHT dht(DHTPIN, DHTTYPE);

bool manualFanOverride = true;  // fan state change
bool lastFanButtonState = HIGH;  // fan state btn of last time
bool manualStateOverride = false;  // system state (automatic or manual)
bool lastStateButtonState = HIGH;  // system state btn of last time

void setup() {
  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(yellowLEDPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(fanButtonPin, INPUT_PULLUP);
  pinMode(stateButtonPin, INPUT_PULLUP);
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  // read temp
  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    // Serial.println("Can't get temperature.");
    return;
  }
  
  bool currentStateButtonState = digitalRead(stateButtonPin);
  if (lastStateButtonState == HIGH && currentStateButtonState == LOW) {
    manualStateOverride = !manualStateOverride;  // change system state
  }
  lastStateButtonState = currentStateButtonState;
  // Serial.print("manualStateOverride: ");
  // Serial.println(manualStateOverride);
  Serial.print("Data: ");
  Serial.print(temperature);
  Serial.print(",");
  Serial.print(manualStateOverride ? "Manual" : "Automatic");
  Serial.print(",");
  digitalWrite(yellowLEDPin, LOW);
  if (!manualStateOverride){
    // temperature control the leds and fan
    if (temperature > 26.0) {
      digitalWrite(fanPin, HIGH);
      digitalWrite(redLEDPin, HIGH);
      digitalWrite(greenLEDPin, LOW);
      if(manualFanOverride == false){
        manualFanOverride = true;
      }
    } else {
      digitalWrite(fanPin, LOW);
      digitalWrite(redLEDPin, LOW);
      digitalWrite(greenLEDPin, HIGH);
      if(manualFanOverride == true){
        manualFanOverride = false;
      }
    }
    
  }else{
    // fan button
    bool currentFanButtonState = digitalRead(fanButtonPin);
    if (lastFanButtonState == HIGH && currentFanButtonState == LOW) {
      manualFanOverride = !manualFanOverride;  // change fan state
    }
    lastFanButtonState = currentFanButtonState;
    if (manualFanOverride){
      digitalWrite(fanPin, HIGH);
      digitalWrite(yellowLEDPin, HIGH);
    }else{
      digitalWrite(fanPin, LOW);
      digitalWrite(yellowLEDPin, HIGH);
    }
  }
  Serial.println(manualFanOverride ? "Fan On" : "Fan Off");
  delay(800);
}