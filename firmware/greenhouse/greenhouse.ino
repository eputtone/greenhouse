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
const prog_char security_passphrase[] PROGMEM = {"xxxxxxx"};	// max 64 characters

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
//const int PIN_RESET = 4;
const int PIN_WATER_BUTTON = 4;
const int PIN_LED = 7;
const int PIN_DHT11 = 6;

int idCounter = 0;

const unsigned long NO_WIFI_REQUEST_AUTOBOOT_DELAY = 600000;
unsigned long lastWifiRequestTime = NO_WIFI_REQUEST_AUTOBOOT_DELAY;
unsigned long loopCounter = 0;
//const unsigned long resetInterval = 3600000;
unsigned long currentTime = 0;

// pump triggering and state variables
const unsigned long PUMPING_ABSORPTION_DELAY = 120000l;
const unsigned long MAX_ALLOWED_PUMPING_TIME = 30000l;
const int MOISTURE_NEEDED_THRESHOLD = 600;
const int SENSOR_DEAD_THRESHOLD = 300;
const int TRIGGER_WATERING_SENSORS_THRESHOLD = 3;
boolean pumpingWater = false;
unsigned long pumpingSinceTime = 0l;
unsigned long pumpingLastTime = -PUMPING_ABSORPTION_DELAY;
unsigned long currentPumpingPeriod = 0l;
unsigned long totalPumpingTime = 0l;

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
  String ending = String(URL);
  if (ending.startsWith("/greenhouse/status")) {
    writeHttpGreenhouseDataJSON();
    return true;
  } else if (ending.startsWith("/greenhouse/counter")) {
    WiServer.print(idCounter++);
    return true;
  } else if (ending.startsWith("/")) {
    writeHttpGreenhouseDataHumanReadable();
    return true;
  }
  // URL not found
  return false;
}

boolean isManualWaterPumping() {
  return digitalRead(PIN_WATER_BUTTON);
}

boolean isWaterBarrelEmpty() {
  int contact = digitalRead(PIN_WATER_METER);
  if (contact == HIGH) {
    return true;
  } else {
    return false;
  }
}

long getPumpingPeriod() {
  int waterNeededCount = 0;
  int moistures[4]; 
  boolean waterNeeded[4];
  moistures[0] = analogRead(PIN_MOISTURE_0);
  moistures[1] = analogRead(PIN_MOISTURE_1);
  moistures[2] = analogRead(PIN_MOISTURE_2);
  moistures[3] = analogRead(PIN_MOISTURE_3);
  for (int i=0; i<4; i++) {
    waterNeeded[i] = moistures[i] > SENSOR_DEAD_THRESHOLD && moistures[i] < MOISTURE_NEEDED_THRESHOLD;
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
  
  // Run WiServer
  WiServer.server_task();
  
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
  
  // reset every now and then, because wifi shield is unstable
  // -- now using watchdog timer
  //currentTime = millis();
  //if (currentTime > resetInterval) {
  //  Serial.println("RESET!");
  //  digitalWrite(PIN_RESET, HIGH);
  //}
  
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
  float temperature = -999.99;
  float humidity = -999.99;
  int chk = DHT11.read(PIN_DHT11);
  switch (chk) {  
    case DHTLIB_OK: 
      humidity = (float)DHT11.humidity;
      temperature = (float)DHT11.temperature;
      break;
  }
  WiServer.print("{\n");
  WiServer.print("\t\"id\": \"");
  WiServer.print(idCounter++);
  WiServer.print("\",\n\t\"sensorData\": {\n\t\t\"waterBarrelEmpty\": ");  
  WiServer.print(isWaterBarrelEmpty()?"true":"false");
  WiServer.print(",\n\t\t\"temperature\": \"");
  WiServer.print(temperature, 2);
  WiServer.print("\",\n\t\t\"humidity\": \"");
  WiServer.print(humidity, 2);
  WiServer.print(",\n\t\t\"totalPumpingTime\": \"");
  WiServer.print(totalPumpingTime
  );  
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

// ************************************************
// SECTION: HTML FORMAT
// ************************************************

void writeHttpGreenhouseDataHumanReadable() {
    WiServer.print("<html>");

    int chk = DHT11.read(PIN_DHT11);
    switch (chk) {  
      case DHTLIB_OK: 
        WiServer.println("OK"); 
        break;
      case DHTLIB_ERROR_CHECKSUM: 
        WiServer.println("Checksum error"); 
        break;
      case DHTLIB_ERROR_TIMEOUT: 
        WiServer.println("Time out error"); 
        break;
      default: 
        WiServer.println("Unknown error"); 
        break;
    }
    WiServer.print("Humidity (%): ");
    WiServer.print((float)DHT11.humidity, 2);
    WiServer.print("<br/>");
    WiServer.print("Temperature (oC): ");
    WiServer.print((float)DHT11.temperature, 2);    
    WiServer.print("<br/>");
    
    if (isWaterBarrelEmpty()) {
      WiServer.print("Water: empty");
    } else {
      WiServer.print("Water: filled");
    }
    WiServer.print("<br/>");
    WiServer.print(getMoistureValue(PIN_MOISTURE_0));
    WiServer.print("<br/>");
    WiServer.print(getMoistureValue(PIN_MOISTURE_1));
    WiServer.print("<br/>");
    WiServer.print(getMoistureValue(PIN_MOISTURE_2));
    WiServer.print("<br/>");
    WiServer.print(getMoistureValue(PIN_MOISTURE_3));
    WiServer.print("<br/>");
    WiServer.print("</html>");
}

String getMoistureValue(int pinNo) {
  int value = analogRead(pinNo);

  // the sensor value description
  // 0  ~300     dry soil
  // 300~700     humid soil
  // 700~950     in water  
  //
  // Probably because of the resistance in sensor wires,
  // my moisturazion threshold is 700

  if (value < 700) {
    String str = "Dry soil: ";
    return str + value;
  } else { 
    String str = "Moist soil: ";
    return str + value;
  }
}



