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
  Serial.println("Wiring: ");
  Serial.print("  Arduino digital pin ");
  Serial.print(RELAY_PIN);
  Serial.println(" to logic input pin of relay module");
  Serial.println("  Arduino VCC to relay module VCC");
  Serial.println("  Arduino GND to relay module logic GND");
  Serial.println("  12V in to Arduino RAW");
  Serial.print("  DS18B20 left pin to GND, middle pin to Arduino digital pin ");
  Serial.print(RELAY_PIN);
  Serial.println(", right pin to VCC");
  Serial.print("  3.6 - 4.7k resistor VCC to Arduino digital pin ");
  Serial.println(RELAY_PIN);
  Serial.println("Optional:");
  Serial.println("  Cut NC relay output pin");
  Serial.println("  Connect that screw terminal to GND");
  Serial.println("  Connect 12V to relay centre pin (and on to Arduino RAW)");
  Serial.println("  Connect fan to pin 1 and 3 of connector block");
  Serial.println("Settings:");
  Serial.print("  Will turn on above ");
  Serial.print(onThreshold);
  Serial.print("C, off below ");
  Serial.print(offThreshold);
  Serial.println("C");
  Serial.println("  Change this in the source and re-upload");
  
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
