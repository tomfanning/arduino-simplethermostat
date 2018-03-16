#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 3
#define RELAY_PIN 12
#define LED_PIN 13

// will turn off below SETPOINT degrees C, on above SETPOINT + HYSTERISIS
#define SETPOINT 30
#define HYSTERISIS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float lastVal;
float thisVal;
int onThreshold = SETPOINT + HYSTERISIS;
int offThreshold = SETPOINT;
int relayState;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Start up the library
  sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  Serial.println("Simple thermostatic fan controller");
  Serial.println("https://github.com/tomfanning/arduino-simplethermostat.git");
  Serial.print("Will turn on above ");
  Serial.print(onThreshold);
  Serial.print("C, off below ");
  Serial.print(offThreshold);
  Serial.print("C");
  
  // do it once without caring about current relay state, so we start up in the right state
  sensors.requestTemperatures();
  thisVal = sensors.getTempCByIndex(0);
  if (thisVal > onThreshold) {
    turnOn();
  } else {
    turnOff();
  }
}

void turnOn(){
  Serial.print("above ");
  Serial.print(onThreshold);
  Serial.println(", turning on");
  digitalWrite(RELAY_PIN, LOW); // relay module inverts logic
  digitalWrite(LED_PIN, HIGH); // relay module inverts logic
  relayState = 1;
}

void turnOff(){
  Serial.print("below ");
  Serial.print(offThreshold);
  Serial.println(", turning off");
  digitalWrite(RELAY_PIN, HIGH); // relay module inverts logic
  digitalWrite(LED_PIN, LOW); // relay module inverts logic
  relayState = 0;
}

void loop() {
  
  sensors.requestTemperatures();
  thisVal = sensors.getTempCByIndex(0);
  
  if (thisVal != lastVal){
    Serial.println(thisVal);
    lastVal = thisVal;
  }

  if (!relayState) {
    // fan is currently off
    if (thisVal > onThreshold) {
      turnOn();
    }
  } else {
    // fan is currently on
    if (thisVal < offThreshold) {
      turnOff();
    }
  }
}
