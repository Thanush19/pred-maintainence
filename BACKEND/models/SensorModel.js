const mongoose = require("mongoose");

const SensorSchema = mongoose.Schema({
  vibration: {
    type: Array,
    required: true,
  },
  rpm: {
    type: Array,
    required: true,
  },
  sound: {
    type: Array,
    required: true,
  },
  temperature: {
    type: Array,
    required: true,
  },
  voltage: {
    type: Array,
    required: true,
  },
  current: {
    type: Array,
    required: true,
  },
});

const SensorData = mongoose.model("SensorData", SensorSchema);

module.exports = SensorData;
