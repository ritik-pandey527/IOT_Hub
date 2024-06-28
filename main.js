// Initialize Firebase
var firebaseConfig = {
  apiKey: "AIzaSyBVBlE_iton_QqHCuLqoKCBtHpv3R-SVKY",
  authDomain: "esp32-home-20456.firebaseapp.com",
  databaseURL: "https://esp32-home-20456-default-rtdb.firebaseio.com",
  projectId: "esp32-home-20456",
  storageBucket: "esp32-home-20456.appspot.com",
  messagingSenderId: "253305084224",
  appId: "1:253305084224:web:8760d40531a05255ab02a5",
};

firebase.initializeApp(firebaseConfig);

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
var sensorDataRef = firebase.database().ref("Sensors/ESP101/");

// Update sensor readings on data changes
sensorDataRef.on("value", function(snapshot) {
  const data = snapshot.val();
  if (data) {
    updateSensorReadings(data);
  } else {
    console.log("No sensor data found in Firebase");
  }
}, function(error) {
  console.log("Error: " + error.code);
});