// Start of wireless configuration parameters ----------------------------------------

#include <WiServer.h>
#include <avr/wdt.h>

#define WIRELESS_MODE_INFRA	1
#define WIRELESS_MODE_ADHOC	2

unsigned char local_ip[] = {192,168,0,173};	// IP address of WiShield
unsigned char gateway_ip[] = {192,168,0,1};	// router or gateway IP address
unsigned char subnet_mask[] = {255,255,255,0};	// subnet mask for the local network
const prog_char ssid[] PROGMEM = {"vauhtikeinu"};		// max 32 bytes

unsigned char security_type = 3;	// 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

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

const int PIN_MOISTURE_0 = A0;
const int PIN_MOISTURE_1 = A1;
const int PIN_MOISTURE_2 = A2;
const int PIN_MOISTURE_3 = A3;
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

#define URL_STATUS "/greenhouse/status"
#define URL_COUNTER "/greenhouse/counter"
#define URL_CONTROL "/greenhouse/control"

// pump triggering and state variables
const unsigned long PUMPING_ABSORPTION_DELAY = 120000l;
const unsigned long MAX_ALLOWED_PUMPING_TIME = 30000l;
int MOISTURE_NEEDED_THRESHOLD_COLD = 600;
int MOISTURE_NEEDED_THRESHOLD_HOT = 650;
const int SENSOR_DEAD_THRESHOLD = 300;
const int TRIGGER_WATERING_SENSORS_THRESHOLD = 3;
int TEMPERATURE_THRESHOLD = 30;
boolean pumpingWater = false;
unsigned long pumpingSinceTime = 0l;
unsigned long pumpingLastTime = -PUMPING_ABSORPTION_DELAY;
unsigned long currentPumpingPeriod = 0l;
unsigned long totalPumpingTime = 0l;
float currentTemperature = -999.99;
float currentHumidity = -999.99;

#include <dht11.h>
dht11 DHT11;

void setup() {
  wdt_disable();
    
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

bool prefix(const char* pre, const char* source)
{
  return strncmp(pre, source, strlen(pre)) == 0;
}

void setParameterValues(char* URL) {
  int value = parseParameterValue(URL, CMD_MOISTURE_LIMIT_COLD);
  if (value > -1 && value < 1000) {
    MOISTURE_NEEDED_THRESHOLD_COLD = value;
  }    
  value = parseParameterValue(URL, CMD_MOISTURE_LIMIT_HOT);
  if (value > -1 && value < 1000) {
    MOISTURE_NEEDED_THRESHOLD_HOT = value;
  }
  value = parseParameterValue(URL, CMD_TEMPERATURE_LIMIT);
  if (value > -100 && value < 100) {
    TEMPERATURE_THRESHOLD = value;
  } 
  WiServer.print("DONE");  
}

int parseParameterValue(char* URL, char* parameterName) {
  int ret = -100;
  char* startIndex = strstr(URL, parameterName);
  if (startIndex == NULL) {
    return ret;
  }
  startIndex += strlen(parameterName) + 1;
  char subbuff[4];
  for (int i=0; i<4; i++) {
    if (isdigit(startIndex[i]) && i<3) {
      subbuff[i] = startIndex[i];
    } else {
      subbuff[i] = '\0';
      break;
    }
  }
  WiServer.print(parameterName);
  WiServer.print("=");
  WiServer.print(subbuff);
  WiServer.print("\n");
  if (subbuff[0] != '\0') {
    ret = atoi(subbuff);
  }
  return ret;
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

long getPumpingPeriod() {
  int waterNeededCount = 0;
  int moistures[4]; 
  boolean waterNeeded[4];
  int moistureNeededThreshold = MOISTURE_NEEDED_THRESHOLD_COLD;
  if (TEMPERATURE_THRESHOLD < ((int)currentTemperature)) {
    moistureNeededThreshold = MOISTURE_NEEDED_THRESHOLD_HOT;
  }
  moistures[0] = analogRead(PIN_MOISTURE_0);
  moistures[1] = analogRead(PIN_MOISTURE_1);
  moistures[2] = analogRead(PIN_MOISTURE_2);
  moistures[3] = analogRead(PIN_MOISTURE_3);
  for (int i=0; i<4; i++) {
    waterNeeded[i] = moistures[i] > SENSOR_DEAD_THRESHOLD && moistures[i] < moistureNeededThreshold;
    if (waterNeeded[i]) {
      waterNeededCount++;  
    }
  }
  if (waterNeededCount >= TRIGGER_WATERING_SENSORS_THRESHOLD) {
    return MAX_ALLOWED_PUMPING_TIME;
  }
  // These pots need water more often (no water container),
  // and short watering period reaches only these pots.
  if (waterNeeded[1] && waterNeeded[3]) {
    return MAX_ALLOWED_PUMPING_TIME / 2;
  }
  return 0l;
}

void loop() {
  currentTime = millis();
  if (lastWifiRequestTime + NO_WIFI_REQUEST_AUTOBOOT_DELAY > currentTime) {
    wdt_reset();
  }
  WiServer.server_task();
  
  if (loopCounter % 100000 == 0) {
    readTemperature();
  }
  
  int newPumpingCmd = LOW;
  long reqPumpingPeriod = getPumpingPeriod();
  if (isManualWaterPumping()) {
    newPumpingCmd = HIGH;
  } else if ((reqPumpingPeriod > 0l || pumpingWater) && !isWaterBarrelEmpty()) {
    if (pumpingWater) {
      if (pumpingSinceTime + currentPumpingPeriod < currentTime) {
        newPumpingCmd = LOW;
      } else {
        newPumpingCmd = HIGH;
      }
      pumpingLastTime = currentTime;
    } else if (pumpingLastTime + PUMPING_ABSORPTION_DELAY < currentTime) {
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
  WiServer.print("{\n");
  WiServer.print("\t\"id\": \"");
  WiServer.print(idCounter++);
  WiServer.print("\",\n\t\"operatingMetrics\":");
  WiServer.print("{\n\t\t\"pumpingTime\": \"");
  WiServer.print(totalPumpingTime);    
  WiServer.print("\",\n\t\t\"uptime\": \"");
  WiServer.print(currentTime);      
  WiServer.print("\"\n\t},\n\t\"operatingParameters\":");
  WiServer.print("{\n\t\t\"moistureLimits\":"); 
  WiServer.print("{\n\t\t\t\"cold\": \"");  
  WiServer.print(MOISTURE_NEEDED_THRESHOLD_COLD);
  WiServer.print("\",\n\t\t\t\"hot\": \"");
  WiServer.print(MOISTURE_NEEDED_THRESHOLD_HOT);
  WiServer.print("\"\n\t\t},\n\t\t\"temperatureLimit\": \"");
  WiServer.print(TEMPERATURE_THRESHOLD);
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
  WiServer.print("\",\n\t\t\"moisture3\": \"");
  WiServer.print(analogRead(PIN_MOISTURE_3));
  WiServer.print("\"\n\t}\n}");
}

