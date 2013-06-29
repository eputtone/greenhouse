// Start of wireless configuration parameters ----------------------------------------

#include <WiServer.h>
#include <avr/wdt.h>

#define WIRELESS_MODE_INFRA	1
#define WIRELESS_MODE_ADHOC	2

unsigned char local_ip[] = {192,168,0,173};	// IP address of WiShield
unsigned char gateway_ip[] = {192,168,0,1};	// router or gateway IP address
unsigned char subnet_mask[] = {255,255,255,0};	// subnet mask for the local network
const prog_char ssid[] PROGMEM = {"kommunistilinja"};		// max 32 bytes

unsigned char security_type = 0;	// 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

// WEP 128-bit keys
// sample HEX keys
prog_uchar wep_keys[] PROGMEM = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,	// Key 0
				  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Key 1
				  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Key 2
				  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	// Key 3
				};

// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = {"xxxxxxxxxx"};	// max 64 characters

// setup the wireless mode
// infrastructure - connect to AP
// adhoc - connect to another WiFi device
unsigned char wireless_mode = WIRELESS_MODE_INFRA;

unsigned char ssid_len;
unsigned char security_passphrase_len;

// End of wireless configuration parameters ----------------------------------------

#include <EEPROM.h>

const int PIN_MOISTURE_0 = A0;
const int PIN_MOISTURE_1 = A1;
const int PIN_MOISTURE_2 = A2;
const int PIN_PUMP = 5;
const int PIN_WATER_METER = 3;
const int PIN_WATER_BUTTON = 4;
const int PIN_LED = 7;
const int PIN_DHT11 = 6;

int idCounter = 0;

const unsigned long NO_WIFI_REQUEST_AUTOBOOT_DELAY = 600000;
unsigned long lastWifiRequestTime = NO_WIFI_REQUEST_AUTOBOOT_DELAY;
unsigned long loopCounter = 0;
unsigned long currentTime = 0;

#define CMD_MOISTURE_LIMIT_COLD "moistureLimitCold"
#define CMD_MOISTURE_LIMIT_HOT "moistureLimitHot"
#define CMD_TEMPERATURE_LIMIT "temperatureLimit"
#define CMD_TIMED_PUMPING_DELAY "timedPumpingDelay"

#define ADDR_MOISTURE_LIMIT_COLD 0
#define ADDR_MOISTURE_LIMIT_HOT 2
#define ADDR_TEMPERATURE_LIMIT 4
#define ADDR_TIMED_PUMPING_DELAY 6

#define URL_STATUS "/greenhouse/status"
#define URL_COUNTER "/greenhouse/counter"
#define URL_CONTROL "/greenhouse/control"

#define MOISTURE_SENSOR_COUNT 3
#define MOISTURE_IN_WATER_THRESHOLD 700

// pump triggering and state variables 
const unsigned long PUMPING_ABSORPTION_DELAY = 600000UL;
const unsigned long DRY_PUMPING_TIME = 30000UL;
const unsigned long TIMED_PUMPING_TIME = 60000UL;
const int SENSOR_DEAD_THRESHOLD = 100;

unsigned int moistureNeededThresholdCold = 600;
unsigned int moistureNeededThresholdHot = 650;
unsigned int temperatureThreshold = 30;
unsigned long timedPumpingDelay = 3600000UL;
boolean pumpingWater = false;
unsigned long pumpingSinceTime = 0UL;
unsigned long pumpingLastTime = 0UL;
unsigned long currentPumpingPeriod = 0UL;
unsigned long totalPumpingTime = 0UL;
float currentTemperature = -999.99;
float currentHumidity = -999.99;

#include <dht11.h>
dht11 DHT11;

void setup() {
  wdt_disable();
  
  moistureNeededThresholdCold = EEPROMReadInt(ADDR_MOISTURE_LIMIT_COLD);
  moistureNeededThresholdHot = EEPROMReadInt(ADDR_MOISTURE_LIMIT_HOT);
  temperatureThreshold = EEPROMReadInt(ADDR_TEMPERATURE_LIMIT);      
  timedPumpingDelay = ((unsigned long)EEPROMReadInt(ADDR_TIMED_PUMPING_DELAY)) * 1000ul;      
        
  //pinMode(PIN_RESET, OUTPUT);
  //digitalWrite(PIN_RESET, LOW);
  pinMode(PIN_WATER_BUTTON, INPUT);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH); 
  pinMode(PIN_WATER_METER, INPUT);
  digitalWrite(PIN_WATER_METER, HIGH);  
  pinMode(PIN_PUMP, OUTPUT);    
  digitalWrite(PIN_PUMP, LOW); 
  
  Serial.begin(57600);
  Serial.println("Init starting..."); 
  
  // Initialize WiServer and have it use the sendMyPage function to serve pages
  WiServer.init(sendMyPage);
  
  // Enable Serial output and ask WiServer to generate log messages (optional)
  WiServer.enableVerboseMode(false);

  // reset by watchdog timer to guard against hang ups
  wdt_enable(WDTO_8S);

  Serial.println("init done!");
}

// This is our page serving function that generates web pages
boolean sendMyPage(char* URL) {
  lastWifiRequestTime = millis();
  if (prefix(URL_STATUS, URL)) {
    writeHttpGreenhouseDataJSON();
    return true;
  } else if (prefix(URL_COUNTER, URL)) {
    WiServer.print(idCounter++);
    return true;
  } else if (prefix(URL_CONTROL, URL)) {
    setParameterValues(URL);
    return true;
  } 
  // URL not found
  return false;
}

bool prefix(const char* pre, const char* source) {
  return strncmp(pre, source, strlen(pre)) == 0;
}

void setParameterValues(char* URL) {
  unsigned int value = parseParameterValue(URL, CMD_MOISTURE_LIMIT_COLD);
  if (value > 0 && value < 1000) {
    moistureNeededThresholdCold = value;
    EEPROMWriteInt(ADDR_MOISTURE_LIMIT_COLD, value);
  }    
  value = parseParameterValue(URL, CMD_MOISTURE_LIMIT_HOT);
  if (value > 0 && value < 1000) {
    moistureNeededThresholdHot = value;
    EEPROMWriteInt(ADDR_MOISTURE_LIMIT_HOT, value);
  }
  value = parseParameterValue(URL, CMD_TEMPERATURE_LIMIT);
  if (value > 0 && value < 100) {
    temperatureThreshold = value;
    EEPROMWriteInt(ADDR_TEMPERATURE_LIMIT, value);    
  } 
  value = parseParameterValue(URL, CMD_TIMED_PUMPING_DELAY);
  if (value > 0 && value < 65535) {
    timedPumpingDelay = ((unsigned long)value) * 1000ul;
    EEPROMWriteInt(ADDR_TIMED_PUMPING_DELAY, value);    
  }   
  WiServer.print("DONE");  
}

unsigned int parseParameterValue(char* URL, char* parameterName) {
  unsigned int ret = 0;
  char* startIndex = strstr(URL, parameterName);
  if (startIndex == NULL) {
    return ret;
  }
  startIndex += strlen(parameterName) + 1;
  char subbuff[6];
  for (int i=0; i<6; i++) {
    if (isdigit(startIndex[i]) && i<5) {
      subbuff[i] = startIndex[i];
    } else {
      subbuff[i] = '\0';
      break;
    }
  }
  if (subbuff[0] != '\0') {
    ret = atoi(subbuff);
  }
  WiServer.print(parameterName);
  WiServer.print("=");
  WiServer.print(ret);
  WiServer.print("\n");  
  return ret;
}

void EEPROMWriteInt(int p_address, unsigned int p_value) {
  byte lowByte = ((p_value >> 0) & 0xFF);
  byte highByte = ((p_value >> 8) & 0xFF);
  EEPROM.write(p_address, lowByte);
  EEPROM.write(p_address + 1, highByte);
}

unsigned int EEPROMReadInt(int p_address) {
  byte lowByte = EEPROM.read(p_address);
  byte highByte = EEPROM.read(p_address + 1);
  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

boolean isManualWaterPumping() {
  return digitalRead(PIN_WATER_BUTTON);
}

boolean isWaterBarrelEmpty() {
  if (digitalRead(PIN_WATER_METER) == HIGH) {
    return true;
  } else {
    return false;
  }
}

void readTemperature() {
  if (DHT11.read(PIN_DHT11) == DHTLIB_OK) {
    currentTemperature = (float)DHT11.temperature;
    currentHumidity = (float)DHT11.humidity;
  }
}

unsigned long getPumpingPeriod() {
  if (isWaterBarrelEmpty()) {
    return 0UL;
  }
  int waterNeededCount = 0;  
  int sensorsAlive = 0;
  unsigned int moistures[MOISTURE_SENSOR_COUNT]; 
  unsigned int moistureNeededThreshold = moistureNeededThresholdCold;
  if (temperatureThreshold < ((unsigned int)currentTemperature)) {
    moistureNeededThreshold = moistureNeededThresholdHot;
  }
  moistures[0] = (unsigned int)analogRead(PIN_MOISTURE_0);
  moistures[1] = (unsigned int)analogRead(PIN_MOISTURE_1);
  moistures[2] = (unsigned int)analogRead(PIN_MOISTURE_2);
  for (int i=0; i<MOISTURE_SENSOR_COUNT; i++) {
    if (moistures[i] > SENSOR_DEAD_THRESHOLD) {
      if (moistures[i] < moistureNeededThreshold) {
        waterNeededCount++;  
      }
      if (moistures[i] > MOISTURE_IN_WATER_THRESHOLD) {
        return 0UL;
      }
      sensorsAlive++;
    }
  }
  if (waterNeededCount == sensorsAlive && sensorsAlive > 0) {
    return DRY_PUMPING_TIME;
  }
  if (currentTime > pumpingLastTime + timedPumpingDelay) {
    return TIMED_PUMPING_TIME;
  }   
  return 0UL;
}

void loop() {
  currentTime = millis();
  if (lastWifiRequestTime + NO_WIFI_REQUEST_AUTOBOOT_DELAY > currentTime) {
    wdt_reset();
  }
  while (WiServer.sendInProgress()) {
    delay(10);
  }
  WiServer.server_task();
  
  if (loopCounter % 100000 == 0) {
    readTemperature();
  }
  
  int newPumpingCmd = LOW;
  unsigned long reqPumpingPeriod = getPumpingPeriod();
  if (isManualWaterPumping()) {
    newPumpingCmd = HIGH;
  } else if (reqPumpingPeriod > 0l || pumpingWater) {
    if (pumpingWater) {
      if (pumpingSinceTime + currentPumpingPeriod < currentTime) {
        newPumpingCmd = LOW;
      } else {
        newPumpingCmd = HIGH;
      }
      pumpingLastTime = currentTime;
    } else if (pumpingLastTime + PUMPING_ABSORPTION_DELAY < currentTime || pumpingLastTime == 0UL) {
      newPumpingCmd = HIGH;
      currentPumpingPeriod = reqPumpingPeriod;
      pumpingSinceTime = currentTime;
      pumpingLastTime = currentTime;
      totalPumpingTime += reqPumpingPeriod;
    }
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

// ************************************************
// SECTION: JSON FORMAT
// ************************************************

void writeHttpGreenhouseDataJSON() {
  WiServer.print("{\n\t\"id\": \"");
  WiServer.print(idCounter++);
  WiServer.print("\",\n\t\"operatingMetrics\":");
  WiServer.print("{\n\t\t\"pumpingTime\": \"");
  WiServer.print(totalPumpingTime);    
  WiServer.print("\",\n\t\t\"uptime\": \"");
  WiServer.print(currentTime);
  WiServer.print("\"\n\t},\n\t\"operatingParameters\":");
  WiServer.print("{\n\t\t\"moistureLimits\":"); 
  WiServer.print("{\n\t\t\t\"cold\": \"");  
  WiServer.print(moistureNeededThresholdCold);
  WiServer.print("\",\n\t\t\t\"hot\": \"");
  WiServer.print(moistureNeededThresholdHot);
  WiServer.print("\"\n\t\t},\n\t\t\"temperatureLimit\": \"");
  WiServer.print(temperatureThreshold);
  WiServer.print("\",\n\t\t\"timedPumpingDelay\": \"");
  WiServer.print(timedPumpingDelay);  
  WiServer.print("\"\n\t},\n\t\"sensors\": ");
  WiServer.print("{\n\t\t\"addWater\": "); 
  WiServer.print(isWaterBarrelEmpty()?"true":"false");
  WiServer.print(",\n\t\t\"temperature\": \"");
  WiServer.print(currentTemperature, 2);
  WiServer.print("\",\n\t\t\"humidity\": \"");
  WiServer.print(currentHumidity, 2);
  WiServer.print("\",\n\t\t\"moisture0\": \"");
  WiServer.print(analogRead(PIN_MOISTURE_0));
  WiServer.print("\",\n\t\t\"moisture1\": \"");
  WiServer.print(analogRead(PIN_MOISTURE_1));
  WiServer.print("\",\n\t\t\"moisture2\": \"");
  WiServer.print(analogRead(PIN_MOISTURE_2));
  WiServer.print("\"\n\t}\n}");
}

