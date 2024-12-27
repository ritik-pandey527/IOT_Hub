# Indoor Air Quality Monitoring System

This project is a complete Air Quality Monitoring (AQM) system that collects real-time environmental data, including temperature, humidity, CO2 levels, particulate matter (PM), and volatile organic compounds (VOC). It features an OLED display for local monitoring, Wi-Fi connectivity for remote data visualization, and Firebase integration for data storage and download.

---

## Features
- **Real-time Data Monitoring**: Measures temperature, humidity, CO2 levels, PM (1.0, 2.5, 10), and VOC index.
- **Firebase Integration**: Stores sensor data in Firebase Realtime Database.
- **CSV Export**: Allows users to download the recorded data as a CSV file.
- **OLED Display**: Displays data, including date, time, temperature, humidity, VOC index, and particulate matter.
- **Wi-Fi Configurations**: Allows users to configure Wi-Fi credentials via a web interface.
- **Access Point Mode**: Hosts an access point for initial configuration or in case of connection failure.
- **NTP Integration**: Fetches and displays real-time clock information.
- **Modular Screens**: Scrollable screens to show detailed information.

---

## Hardware Requirements
- ESP32
- OLED Display (SH1106)
- DHT22 Sensor
- MH-Z19B CO2 Sensor
- PMS5003 Particulate Matter Sensor
- SGP40 VOC Sensor
- Wi-Fi Access

---

## Software Requirements
- Arduino IDE
- Firebase ESP Client Library
- Adafruit GFX Library
- Adafruit SH1106 Library
- DFRobot SGP40 Library
- PMS Library

---

## Setup and Configuration

1. **Firebase Configuration**
   - Create a Firebase project.
   - Enable Realtime Database and set the rules to allow read/write.
   - Replace the Firebase configuration in the code with your project details:
     ```javascript
     const firebaseConfig = {
       apiKey: "YOUR_API_KEY",
       authDomain: "YOUR_PROJECT_ID.firebaseapp.com",
       databaseURL: "https://YOUR_PROJECT_ID.firebaseio.com",
       projectId: "YOUR_PROJECT_ID",
       storageBucket: "YOUR_PROJECT_ID.appspot.com",
       messagingSenderId: "YOUR_SENDER_ID",
       appId: "YOUR_APP_ID"
     };
     ```

2. **Wi-Fi Configuration**
   - On first startup or when no Wi-Fi credentials are stored, the ESP32 starts in Access Point mode.
   - Connect to the access point and configure Wi-Fi via the web interface.

3. **Arduino Libraries**
   - Install all required libraries through Arduino Library Manager or download them from GitHub.

4. **NTP Configuration**
   - Modify the NTP server details if necessary:
     ```cpp
     const char* ntpServer = "pool.ntp.org";
     const long gmtOffset_sec = 0;
     const int daylightOffset_sec = 3600;
     ```

5. **Sensor Wiring**
   - Connect the sensors to the ESP32 as per the following pin mappings:
     - DHT22: GPIO 15
     - MH-Z19B: RX2 (GPIO 16), TX2 (GPIO 17)
     - PMS5003: Serial (configurable)
     - SGP40: I2C (SDA: GPIO 21, SCL: GPIO 22)
     - Relay: GPIO 4

---

## Usage

### Local Monitoring
- View data on the OLED display.
- Scroll through different screens using the timer-based update.

### Remote Monitoring
- Real-time data is pushed to Firebase.
- Data can be downloaded as a CSV file for analysis.

### Wi-Fi Configuration
- Access the web page hosted on the ESP32 to input SSID and password.

### CSV Download
- Use the "Download" button in the web application to save the data locally.

---

## Screens
1. **Date and Time**
2. **Temperature and Humidity**
3. **VOC Index**
4. **Particulate Matter Levels**

---

## Code Structure

### JavaScript (Frontend)
- Handles Firebase initialization, real-time updates, and CSV generation.

### Arduino (Firmware)
- Initializes sensors, Wi-Fi, and Firebase.
- Reads data from sensors and uploads to Firebase.
- Updates the OLED display and serves the configuration web page.

---

## Future Improvements
- Add more advanced analytics and visualizations in the frontend.
- Implement energy-saving modes for sensors and display.
- Integrate additional sensors for broader environmental monitoring.

---

## License
This project is licensed under the MIT License. Feel free to use, modify, and distribute.
