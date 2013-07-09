
#include <avr/wdt.h>

const int PIN_PUMP = 5;
const int PIN_WATER_BUTTON = 4;
const int PIN_LED = 7;

unsigned long loopCounter = 0;
unsigned long currentTime = 0;

const unsigned long TIMED_PUMPING_TIME = 15000UL;
const unsigned long TIMED_PUMPING_DELAY = 7200000UL;
const unsigned long BUTTON_PUMPING_DELAY = 43200000UL;

boolean pumpingWater = false;
boolean manualPumping = false;
unsigned long pumpingSinceTime = 0UL;
unsigned long nextPumpingTime = TIMED_PUMPING_DELAY;

void setup() {
  pinMode(PIN_WATER_BUTTON, INPUT);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH); 
  pinMode(PIN_PUMP, OUTPUT);    
  digitalWrite(PIN_PUMP, LOW); 
  wdt_enable(WDTO_8S);  
  wdt_reset();
}

boolean isManualWaterPumping() {
  return digitalRead(PIN_WATER_BUTTON);
}

void loop() {
  wdt_reset();  
  currentTime = millis();
  int newPumpingCmd = LOW;
  if (isManualWaterPumping()) {
    newPumpingCmd = HIGH;
    manualPumping = true;
  } else if (manualPumping) {
    newPumpingCmd = LOW;    
    manualPumping = false;
    nextPumpingTime = currentTime + BUTTON_PUMPING_DELAY;
  } else if (pumpingWater) {
    if (pumpingSinceTime + TIMED_PUMPING_TIME < currentTime) {
      newPumpingCmd = LOW;
      nextPumpingTime = currentTime + TIMED_PUMPING_DELAY;
    } else {
      newPumpingCmd = HIGH;
    }
  } else if (nextPumpingTime < currentTime) {
    newPumpingCmd = HIGH;
    pumpingSinceTime = currentTime;
  }
  pumpingWater = newPumpingCmd;
  digitalWrite(PIN_PUMP, newPumpingCmd);
  
  delay(10); 
  loopCounter++;
  if (loopCounter % 100 == 0) {
    digitalWrite(PIN_LED, LOW);
  } else if (loopCounter % 50 == 0) { 
    digitalWrite(PIN_LED, HIGH); 
  }
}

