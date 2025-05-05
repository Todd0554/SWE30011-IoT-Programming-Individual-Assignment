#define tempSensorPin A0
#define fanButtonPin 6
#define stateButtonPin 7

const int redLEDPin = 2;
const int greenLEDPin = 3;
const int yellowLEDPin = 4;
const int fanPin = 5;

bool manualFanOverride = true;
bool lastFanButtonState = HIGH;
bool manualStateOverride = false;
bool lastStateButtonState = HIGH;

void setup() {
  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(yellowLEDPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(fanButtonPin, INPUT_PULLUP);
  pinMode(stateButtonPin, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  int analogValue = analogRead(tempSensorPin);
  float voltage = analogValue * (5.0 / 1023.0);       // Convert to voltage
  float temperature = voltage * 100.0;                // LM35: 10mV = 1°C

  if (temperature < -10.0 || temperature > 80.0) {
    return;
  }

  bool currentStateButtonState = digitalRead(stateButtonPin);
  if (lastStateButtonState == HIGH && currentStateButtonState == LOW) {
    manualStateOverride = !manualStateOverride;
  }
  lastStateButtonState = currentStateButtonState;

  Serial.print("DATA:");
  Serial.print(temperature);
  Serial.print(",");
  Serial.print(manualStateOverride ? "Manual" : "Automatic");
  Serial.print(",");

  digitalWrite(yellowLEDPin, LOW);
  if (!manualStateOverride){
    if (temperature > 28.0) {
      digitalWrite(fanPin, HIGH);
      digitalWrite(greenLEDPin, LOW);
      digitalWrite(redLEDPin, HIGH);
      if(manualFanOverride == false){
        manualFanOverride = true;
      }
    } else {
      digitalWrite(fanPin, LOW);
      digitalWrite(greenLEDPin, HIGH);
      digitalWrite(redLEDPin, LOW);
      if(manualFanOverride == true){
        manualFanOverride = false;
      }
    }
  } else {
    bool currentFanButtonState = digitalRead(fanButtonPin);
    if (lastFanButtonState == HIGH && currentFanButtonState == LOW) {
      manualFanOverride = !manualFanOverride;
    }
    lastFanButtonState = currentFanButtonState;
    if (manualFanOverride){
      digitalWrite(fanPin, HIGH);
      digitalWrite(yellowLEDPin, HIGH);
    } else {
      digitalWrite(fanPin, LOW);
      digitalWrite(yellowLEDPin, HIGH);
    }
  }

  Serial.println(manualFanOverride ? "On" : "Off");

  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "FAN_TOGGLE") {
      manualFanOverride = !manualFanOverride;
    } else if (command == "MODE_TOGGLE") {
      manualStateOverride = !manualStateOverride;
    }
  }
  delay(800);
}
