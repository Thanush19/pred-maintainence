import express from "express";
import cors from "cors";
const app = express();
app.use(cors());
const port = 3000;
import { initializeApp } from "firebase/app";
import { getDatabase, onValue, ref } from "firebase/database";

const firebaseConfig = {
  apiKey: "AIzaSyB0IPIrKRugmHUrgdspmcFZkButkKctHkQ",
  authDomain: "predictive-maintenance-4020e.firebaseapp.com",
  databaseURL:
    "https://predictive-maintenance-4020e-default-rtdb.firebaseio.com",
  projectId: "predictive-maintenance-4020e",
  storageBucket: "predictive-maintenance-4020e.appspot.com",
  messagingSenderId: "584970417111",
  appId: "1:584970417111:web:6fdae09ab92b0039fa1192",
};

// Initialize Firebase
const firebaseapp = initializeApp(firebaseConfig);
const db = getDatabase();
const dbRefSensor = ref(db, "sensor");
const dbRefHealth = ref(db, "health");

let sensorData = {
  vibration: 0,
  rpm: 0,
  sound: 0,
  temperature: 0,
  voltage: 0,
  current: 0,
};

let healthData = {
  beatsPerMinute: 0,
  spo2: 0,
  temperature: 0,
  ecg: 0,
};

onValue(dbRefSensor, (snapshot) => {
  const data = snapshot.val();
  console.log(data);
  sensorData = data;
});

onValue(dbRefHealth, (snapshot) => {
  const data = snapshot.val();
  console.log(data);
  healthData = data;
});

app.get("/", (req, res) => {
  res.send("RUNNING SUCESSFUL");
});

app.get("/data", (req, res) => {
  res.send(sensorData);
});

app.get("/health", (req, res) => {
  res.send(healthData);
});

app.listen(port, () => {
  console.log(`Example app listening on port ${port}`);
});
