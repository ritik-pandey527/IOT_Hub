#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include <WiFi.h>
#include <time.h>
#include <WebServer.h>
#include <Preferences.h>
#include <MHZ19.h>
#include <HardwareSerial.h> 
#include <DHT.h>
#include <DFRobot_SGP40.h>
#include <PMS.h>
#include <Arduino.h>
#include <Firebase_ESP_Client.h>

#define DHTPIN 15
#define DHTTYPE DHT22

float humidity;
float temperature;
int co2;
int pms1;
int pms25;
int pms10;
uint16_t myindex;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
const int espnum = 101;

const char* apSSID = "AQD-ESP01";
const char* apPassword = "12345678";
const int resetPin = 5; // GPIO 5

WebServer server(80);
Preferences preferences;

// NTP Server Details
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;

// OLED Display
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define I2Cdisplay_SDA 21
#define I2Cdisplay_SCL 22

Adafruit_SH1106 display;

// Screens
int displayScreenNum = 0;
int displayScreenNumMax = 4;

unsigned long lastTimer = 0;
unsigned long timerDelay = 3000;

unsigned char temperature_icon[] = {
  0b00000001, 0b11000000,  //        ###
  0b00000011, 0b11100000,  //       #####
  0b00000111, 0b00100000,  //      ###  #
  0b00000111, 0b11100000,  //      ######
  0b00000111, 0b00100000,  //      ###  #
  0b00000111, 0b11100000,  //      ######
  0b00000111, 0b00100000,  //      ###  #
  0b00000111, 0b11100000,  //      ######
  0b00000111, 0b00100000,  //      ###  #
  0b00001111, 0b11110000,  //     ########
  0b00011111, 0b11111000,  //    ##########
  0b00011111, 0b11111000,  //    ##########
  0b00011111, 0b11111000,  //    ##########
  0b00011111, 0b11111000,  //    ##########
  0b00001111, 0b11110000,  //     ########
  0b00000111, 0b11100000,  //      ######
};

unsigned char humidity_icon[] = {
  0b00000000, 0b00000000,  //
  0b00000001, 0b10000000,  //        ##
  0b00000011, 0b11000000,  //       ####
  0b00000111, 0b11100000,  //      ######
  0b00001111, 0b11110000,  //     ########
  0b00001111, 0b11110000,  //     ########
  0b00011111, 0b11111000,  //    ##########
  0b00011111, 0b11011000,  //    ####### ##
  0b00111111, 0b10011100,  //   #######  ###
  0b00111111, 0b10011100,  //   #######  ###
  0b00111111, 0b00011100,  //   ######   ###
  0b00011110, 0b00111000,  //    ####   ###
  0b00011111, 0b11111000,  //    ##########
  0b00001111, 0b11110000,  //     ########
  0b00000011, 0b11000000,  //       ####
  0b00000000, 0b00000000,  //
};

// Create display marker for each screen
void displayIndicator(int displayNumber) {
  int xCoordinates[5] = { 44, 54, 64, 74};
  for (int i = 0; i < 4; i++) {
    if (i == displayNumber) {
      display.fillCircle(xCoordinates[i], 60, 2, WHITE);
    } else {
      display.drawCircle(xCoordinates[i], 60, 2, WHITE);
    }
  }
}

// SCREEN NUMBER 0: DATE AND TIME
void displayLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  //GET DATE
  //Get full weekday name
  char weekDay[10];
  strftime(weekDay, sizeof(weekDay), "%a", &timeinfo);
  //Get day of month
  char dayMonth[4];
  strftime(dayMonth, sizeof(dayMonth), "%d", &timeinfo);
  //Get abbreviated month name
  char monthName[5];
  strftime(monthName, sizeof(monthName), "%b", &timeinfo);
  //Get year
  char year[6];
  strftime(year, sizeof(year), "%Y", &timeinfo);

  //GET TIME
  //Get hour (24 hour format)
  char hour[4];
  strftime(hour, sizeof(hour), "%H", &timeinfo);
  //Get minute
  char minute[4];
  strftime(minute, sizeof(minute), "%M", &timeinfo);

  //Display Date and Time on OLED display
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.setCursor(19, 5);
  display.print(hour);
  display.print(":");
  display.print(minute);
  display.setTextSize(1);
  display.setCursor(16, 40);
  display.print(weekDay);
  display.print(", ");
  display.print(dayMonth);
  display.print(" ");
  display.print(monthName);
  display.print(" ");
  display.print(year);
  displayIndicator(displayScreenNum);
  display.display();
}

// SCREEN NUMBER 1: TEMPERATURE AND HUMIDITY
void displayTemperature() {
  display.clearDisplay();
  display.setTextSize(2);
  display.drawBitmap(15, 5, temperature_icon, 16, 16, 1);
  display.setCursor(35, 5);
  display.print(temperature);
  display.cp437(true);
  display.setTextSize(1);
  display.print(" ");
  display.write(167);
  display.print("C");
  display.setCursor(0, 34);
  display.setTextSize(1);
  display.print("Humidity: ");
  display.print(humidity);
  display.print(" %");
  displayIndicator(displayScreenNum);
  display.display();
}

// SCREEN NUMBER 2: VOC
void displayVOC() {
  display.clearDisplay();
  display.setTextSize(2);
  display.drawBitmap(15, 5, humidity_icon, 16, 16, 1);
  display.setCursor(35, 5);
  display.print(co2);
  display.print(" PPM");
  display.setCursor(0, 34);
  display.setTextSize(1);
  display.print("VOC Index: ");
  display.print(myindex);
  displayIndicator(displayScreenNum);
  display.display();
}

// SCREEN NUMBER 3: PARTICULATE MATTER
void displayPM() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10, 10);
  display.print("PM1.0: ");
  display.print(pms1);
  display.print(" ug/m3");
  display.setCursor(10, 30);
  display.print("PM2.5: ");
  display.print(pms25);
  display.print(" ug/m3");
  display.setCursor(10, 50);
  display.print("PM10 : ");
  display.print(pms10);
  display.print(" ug/m3");
  displayIndicator(displayScreenNum);
  display.display();
}

// Display the right screen accordingly to the displayScreenNum
void updateScreen() {
  if (WiFi.status() != WL_CONNECTED) {
    displayStatusMessage("Connecting...");
  } else {
    if (displayScreenNum == 0) {
      displayLocalTime();
    } else if (displayScreenNum == 1) {
      displayTemperature();
    } else if (displayScreenNum == 2) {
      displayVOC();
    } else if (displayScreenNum == 3) {
      displayPM();
    } else {
      displayLocalTime();
    }
  }
}

#define RELAY_PIN 4

DFRobot_SGP40 mySgp40(&Wire);

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define API_KEY "TiCg2BgSfzjdFY3adcaSXUvdiThBDncqil3qmIhp"
#define DATABASE_URL "https://indoor-aqm-default-rtdb.firebaseio.com"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

#define RXD2 16
#define TXD2 17

DHT dht(DHTPIN, DHTTYPE);

HardwareSerial SerialPort(0);
MHZ19 myMHZ19;

unsigned long lastDisplayChangeTime = 0;
bool displayPMData = true;
bool relayState = false;

unsigned long co2ZeroStartTime = 0;
bool co2ZeroStarted = false;

struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};

struct pms5003data data;

String htmlPage = "<!DOCTYPE html>\
<html>\
<head><title>ESP32 WiFi Setup</title></head>\
<body>\
<h2>Enter Wi-Fi Credentials</h2>\
<form action=\"/submit\" method=\"POST\">\
  SSID:<br>\
  <input type=\"text\" name=\"ssid\"><br>\
  Password:<br>\
  <input type=\"password\" name=\"password\"><br><br>\
  <input type=\"submit\" value=\"Submit\">\
</form>\
</body>\
</html>";

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleSubmit() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    Serial.println("Received SSID: " + ssid);
    Serial.println("Received Password: " + password);

    // Save the credentials to preferences
    preferences.begin("wifi", false);
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    preferences.end();

    server.send(200, "text/html", "Credentials received, ESP32 will now try to connect to the Wi-Fi.");

    // Attempt to connect to the received Wi-Fi credentials
    WiFi.begin(ssid.c_str(), password.c_str());

    int attempts = 0;
    const int maxAttempts = 30; // Increase the number of attempts for a better chance to connect
    while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
      delay(1000);
      Serial.print(".");
      displayStatusMessage("Connecting...");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected to Wi-Fi!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      server.send(200, "text/html", "Successfully connected to Wi-Fi. IP Address: " + WiFi.localIP().toString());

      // Stop the Access Point
      WiFi.softAPdisconnect(true);
      Serial.println("Access Point stopped.");
      displayStatusMessage("Connected!");
      delay(2000); // Display the "Connected!" message for 2 seconds

      // Init and get the time
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    } else {
      Serial.println("Failed to connect to Wi-Fi.");
      server.send(200, "text/html", "Failed to connect to Wi-Fi. Please try again.");
      displayStatusMessage("Failed to connect");
      delay(2000); // Display the "Failed to connect" message for 2 seconds
      startAPMode(); // Restart AP mode if connection fails
    }
  } else {
    server.send(400, "text/html", "Invalid request");
  }
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial2.begin(115200);
  dht.begin();
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(26, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(26, LOW);

  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);

  pinMode(resetPin, INPUT_PULLUP);

  preferences.begin("wifi", true);
  String storedSSID = preferences.getString("ssid", "");
  String storedPassword = preferences.getString("password", "");
  preferences.end();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Wire.begin();

  SerialPort.begin(9600, SERIAL_8N1, 3, 1);
  myMHZ19.begin(SerialPort);
  myMHZ19.autoCalibration();

  if (!mySgp40.begin()) {
    Serial.println("SGP40 initialization failed");
    while (1)
      delay(10);
  }

  if (storedSSID.length() > 0 && storedPassword.length() > 0) {
    Serial.print("Connecting to stored SSID: ");
    Serial.println(storedSSID);
    WiFi.begin(storedSSID.c_str(), storedPassword.c_str());

    int attempts = 0;
    const int maxAttempts = 30;
    while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
      delay(1000);
      Serial.print(".");
      displayStatusMessage("AP Mode");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected to Wi-Fi!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      displayStatusMessage("Connected!");
      delay(2000); // Display the "Connected!" message for 2 seconds
      // Init and get the time
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      return;
    } else {
      Serial.println("Failed to connect to stored Wi-Fi. Starting AP mode.");
      displayStatusMessage("AP Mode");
    }
  }

  // Start AP mode if no stored credentials or failed to connect
  startAPMode();
}

void loop() {
  server.handleClient();

  // Check if the reset button is held down
  static unsigned long buttonPressTime = 0;
  if (digitalRead(resetPin) == LOW) {
    if (buttonPressTime == 0) {
      buttonPressTime = millis();
    } else if (millis() - buttonPressTime > 5000) {
      // Button held for more than 5 seconds, reset the Wi-Fi credentials
      preferences.begin("wifi", false);
      preferences.clear();
      preferences.end();
      Serial.println("Wi-Fi credentials erased. Restarting in AP mode...");
      ESP.restart();
    }
  } else {
    buttonPressTime = 0;
  }

  if ((millis() - lastTimer) > timerDelay) {
    updateScreen();
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println(displayScreenNum);
      if (displayScreenNum < displayScreenNumMax) {
        displayScreenNum++;
      } else {
        displayScreenNum = 0;
      }
    }
    lastTimer = millis();
  }

  unsigned long currentTime = millis();
  co2 = myMHZ19.getCO2();
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  if (co2 == 0) {
    if (!co2ZeroStarted) {
      co2ZeroStartTime = currentTime;
      co2ZeroStarted = true;
    } else if (currentTime - co2ZeroStartTime > 5000) {
      Serial.println("CO2 reading is zero for more than 5 seconds, restarting ESP...");
      esp_restart();
    }
  } else {
    co2ZeroStarted = false;
  }
  if (currentTime - lastDisplayChangeTime >= 3000) {
    lastDisplayChangeTime = currentTime;
    displayPMData = !displayPMData;
    display.clearDisplay();
  }
  if (displayPMData) {
    Serial.println("Reading PMS Data...");
    if (readPMSdata(&Serial1)) {
      Serial.println("Air Quality Monitor");
      Serial.print("PM1.0 :");
      pms1 = data.pm10_standard;
      Serial.print(pms1);
      Serial.println(" (ug/m3)");
      Serial.print("PM2.5 :");
      pms25 = data.pm25_standard;
      Serial.print(pms25);
      Serial.println(" (ug/m3)");
      Serial.print("PM10  :");
      pms10 = data.pm100_standard;
      Serial.print(pms10);
      Serial.println(" (ug/m3)");

      if (data.pm25_standard > 50) {
        digitalWrite(RELAY_PIN, HIGH);
        digitalWrite(26, HIGH);
        relayState = true;
        Serial.println("Relay is ON due to PM2.5");
      } else if (!relayState) {
        digitalWrite(RELAY_PIN, LOW);
        digitalWrite(26, LOW);
        Serial.println("Relay is OFF due to PM2.5");
      }
    } else {
      Serial.println("Failed to read PMS data");
    }
  } else {
    Serial.print("CO2: ");
    Serial.println(co2);
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("C, Humidity: ");
    Serial.print(humidity);
    Serial.println("%");

    if (co2 > 800) {
      digitalWrite(RELAY_PIN, HIGH);
      digitalWrite(26, HIGH);
      relayState = true;
      Serial.println("Relay is ON due to CO2");
    } else if (co2 <= 800) {
      relayState = false;
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(26, LOW);
      Serial.println("Relay is OFF due to CO2");
    }
  }

  myindex = mySgp40.getVoclndex();

  Serial.print("vocIndex = ");
  Serial.println(myindex);
  sendToFirebase(espnum, humidity, temperature, co2, pms1, pms25, pms10, myindex);
}

void startAPMode() {
  WiFi.softAP(apSSID, apPassword);
  Serial.println("Access Point started");
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/submit", HTTP_POST, handleSubmit);
  server.begin();
  Serial.println("HTTP server started");
}

void displayStatusMessage(const char* message) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print(message);
  display.display();
}

boolean readPMSdata(Stream *s) {
  if (!s->available()) {
    return false;
  }

  if (s->peek() != 0x42) {
    s->read();
    return false;
  }

  if (s->available() < 32) {
    return false;
  }

  uint8_t buffer[32];
  uint16_t sum = 0;
  s->readBytes(buffer, 32);

  for (uint8_t i = 0; i < 30; i++) {
    sum += buffer[i];
  }

  uint16_t buffer_u16[15];
  for (uint8_t i = 0; i < 15; i++) {
    buffer_u16[i] = buffer[2 + i * 2 + 1];
    buffer_u16[i] += (buffer[2 + i * 2] << 8);
  }

  memcpy((void *)&data, (void *)buffer_u16, 30);

  if (sum != data.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  return true;
}

void sendToFirebase(int espnum, float humidity, float temperature, int co2, int pms1, int pms25, int pms10, int myindex) {
  if (Firebase.ready() && signupOK) {
    String path = "Sensors/ESP" + String(espnum);

    if (!Firebase.RTDB.setFloat(&fbdo, path + "/DHT11/Humidity", humidity)) {
      Serial.println("Failed to send humidity");
      Serial.println("Reason: " + fbdo.errorReason());
    }
    if (!Firebase.RTDB.setFloat(&fbdo, path + "/DHT11/Temperature", temperature)) {
      Serial.println("Failed to send temperature");
      Serial.println("Reason: " + fbdo.errorReason());
    }

    if (!Firebase.RTDB.setInt(&fbdo, path + "/MH-Z19B/CO2", co2)) {
      Serial.println("Failed to send CO2");
      Serial.println("Reason: " + fbdo.errorReason());
    }

    if (!Firebase.RTDB.setInt(&fbdo, path + "/PMS5003/PMS1_0", pms1)) {
      Serial.println("Failed to send PMS1.0");
      Serial.println("Reason: " + fbdo.errorReason());
    }
    if (!Firebase.RTDB.setInt(&fbdo, path + "/PMS5003/PMS2_5", pms25)) {
      Serial.println("Failed to send PMS2.5");
      Serial.println("Reason: " + fbdo.errorReason());
    }
    if (!Firebase.RTDB.setInt(&fbdo, path + "/PMS5003/PMS10", pms10)) {
      Serial.println("Failed to send PMS10");
      Serial.println("Reason: " + fbdo.errorReason());
    }
    if (!Firebase.RTDB.setInt(&fbdo, path + "/TVOC/Index", myindex)) {
      Serial.println("Failed to send PMS10");
      Serial.println("Reason: " + fbdo.errorReason());
    }
  }
}
