// Import the functions you need from the SDKs you need
import { initializeApp } from "https://www.gstatic.com/firebasejs/10.12.5/firebase-app.js";
import { getDatabase, ref, onValue, set } from "https://www.gstatic.com/firebasejs/10.12.5/firebase-database.js";

// Your web app's Firebase configuration
const firebaseConfig = {
  apiKey: "AIzaSyAuLO4N3Ay9YhIvh9e8vWpMCwxZajTipAE",
  authDomain: "outdoor-aqd.firebaseapp.com",
  databaseURL: "https://outdoor-aqd-default-rtdb.firebaseio.com",
  projectId: "outdoor-aqd",
  storageBucket: "outdoor-aqd.appspot.com",
  messagingSenderId: "664373314974",
  appId: "1:664373314974:web:e9056029dd47539330c28f"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

// Function to update sensor readings
function updateSensorReadings(data) {
  document.getElementById("temp").innerText = data.DHT11.Temperature || "N/A";
  document.getElementById("hum").innerText = data.DHT11.Humidity || "N/A";
  document.getElementById("co2").innerText = data['MH-Z19B'].CO2 || "N/A";
  document.getElementById("pms1_0").innerText = data.PMS5003.PMS1_0 || "N/A";
  document.getElementById("pms2_5").innerText = data.PMS5003.PMS2_5 || "N/A";
  document.getElementById("pms10").innerText = data.PMS5003.PMS10 || "N/A";
  document.getElementById("voc").innerText = data.TVOC.Index || "N/A";
}

// Reference to sensor data in Firebase
const sensorDataRef = ref(database, "Sensors/ESP101/");

// Update sensor readings on data changes
onValue(sensorDataRef, (snapshot) => {
  const data = snapshot.val();
  if (data) {
    updateSensorReadings(data);
  } else {
    console.log("No sensor data found in Firebase");
  }
}, (error) => {
  console.log("Error: " + error.code);
});

// Function to generate random sensor data
function generateRandomSensorData() {
  return {
    DHT11: {
      Temperature: (Math.random() * 30 + 15).toFixed(2), // Random temperature between 15°C and 45°C
      Humidity: (Math.random() * 40 + 30).toFixed(2)    // Random humidity between 30% and 70%
    },
    'MH-Z19B': {
      CO2: Math.floor(Math.random() * 400 + 400) // Random CO2 level between 400 and 800
    },
    PMS5003: {
      PMS1_0: Math.floor(Math.random() * 50 + 5),  // Random PM1.0 value between 5 and 55
      PMS2_5: Math.floor(Math.random() * 50 + 5),  // Random PM2.5 value between 5 and 55
      PMS10: Math.floor(Math.random() * 50 + 5)    // Random PM10 value between 5 and 55
    },
    TVOC: {
      Index: Math.floor(Math.random() * 100 + 10)  // Random TVOC Index between 10 and 110
    }
  };
}

// Function to update Firebase with random data
function updateFirebaseWithRandomData() {
  const randomData = generateRandomSensorData();
  set(sensorDataRef, randomData)
    .then(() => {
      console.log("Data updated successfully");
    })
    .catch((error) => {
      console.log("Data could not be written: " + error.message);
    });
}

// Update Firebase with random data every 5 seconds
setInterval(updateFirebaseWithRandomData, 5000);
