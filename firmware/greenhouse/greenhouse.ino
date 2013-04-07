// Start of wireless configuration parameters ----------------------------------------

#include <WiServer.h>

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
const prog_char security_passphrase[] PROGMEM = {"xxxxxx"};	// max 64 characters

// setup the wireless mode
// infrastructure - connect to AP
// adhoc - connect to another WiFi device
unsigned char wireless_mode = WIRELESS_MODE_INFRA;

unsigned char ssid_len;
unsigned char security_passphrase_len;

// End of wireless configuration parameters ----------------------------------------

const int PIN_MOISTURE_1 = 0;
const int PIN_MOISTURE_2 = 1;

const int PIN_PUMP = 4;
const int PIN_WATER_METER = 3;

int idCounter = 0;

#include <dht11.h>
dht11 DHT11;
const int PIN_DHT11 = 5;

// This is our page serving function that generates web pages
boolean sendMyPage(char* URL) {
  
  if (strcmp(URL, "/") == 0) {
    writeHttpGreenhouseDataHumanReadable();
    return true;
  } else if (strcmp(URL, "/greenhouse/status") == 0) {
    writeHttpGreenhouseDataJSON();
    return true;
  } else if (strcmp(URL, "/greenhouse/counter") == 0) {
    writeHttpResponseNumber();    
    return true;
  }
  // URL not found
  return false;
}

boolean isWaterBarrelEmpty() {
  int contact = digitalRead(PIN_WATER_METER);
  if (contact == LOW) {
    return true;
  } else {
    return false;
  }
}

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
  WiServer.print("\",\n\t\t\"moisture1\": \"");
  WiServer.print(analogRead(PIN_MOISTURE_1));
  WiServer.print("\",\n\t\t\"moisture2\": \"");
  WiServer.print(analogRead(PIN_MOISTURE_2));
  WiServer.print("\"\n\t}\n}");
}

void writeHttpResponseNumber() {
  WiServer.print(idCounter++);
}

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
    WiServer.print(getMoistureValue(PIN_MOISTURE_1));
    WiServer.print("<br/>");
    WiServer.print(getMoistureValue(PIN_MOISTURE_2));
    WiServer.print("<br/>");
    WiServer.print("</html>");
}

String getMoistureValue(int pinNo) {
  int value = analogRead(pinNo);

  // the sensor value description
  // 0  ~300     dry soil
  // 300~700     humid soil
  // 700~950     in water  

  if (value < 300) {
    String str = "Dry soil: ";
    return str + value;
  } else if (value < 700) {
    String str = "Humid soil: "; 
    return str + value;
  } else { 
    String str = "In the water: ";
    return str + value;
  }
}

void setup() {
  Serial.begin(57600);
  Serial.println("Init starting...");

  pinMode(PIN_WATER_METER, INPUT);
  pinMode(PIN_WATER_METER, HIGH);  
  pinMode(PIN_PUMP, OUTPUT);    
  digitalWrite(PIN_PUMP, LOW); 
  
  // Initialize WiServer and have it use the sendMyPage function to serve pages
  WiServer.init(sendMyPage);
  
  // Enable Serial output and ask WiServer to generate log messages (optional)
  WiServer.enableVerboseMode(true);

  Serial.println("init done!");
}

void loop() {
  // Run WiServer
  WiServer.server_task();
  
  if (isWaterBarrelEmpty()) {
    digitalWrite(PIN_PUMP, LOW);
  } else { 
    digitalWrite(PIN_PUMP, HIGH);
  }
  
}



