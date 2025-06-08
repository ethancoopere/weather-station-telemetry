// This class handle all function to init, get and process temperature and humidity sensor data (DHT22)
// Sensor specification :
//   - Input voltage : 3.3 - 6 V
//   - Measuring range for humidity : 5% - 99% RH
//   - Measuring range for temperature : -40 - 80 °C
//   - Accuracy for humidity : ±2% RH
//   - Accuracy for temperature : ±0.5 °C
//   - Resolution for humidity : 0.1% RH
//   - Resolution for temperature : 0.1 °C

#include <DHT.h>
#define connectionPIN 2
#define dhtType DHT22

#define humidityMinRange 0.0 // (%)
#define humidityMaxRange 100.0 // (%)

#define temperatureMinRange -50.0 // (°C)
#define temperatureMaxRange 60.0 // (°C)

int temperatureState = 0;
int humidityState = 0;

DHT dht(connectionPIN, dhtType);

struct data {
  float value;
  String errorMessage;
  bool success;
};

struct initData {
  int temperatureSensorUp;
  int HumiditySensorUp;
  bool success;
};

class TemperatureAndHumiditySensor
{
  public:
    initData initSensor();
    initData getSensorState();
    data getSensorData(String dataType);
};

initData TemperatureAndHumiditySensor::initSensor() {

  // Init of DHT22 sensor
  dht.begin();

  // Get data from the sensor just one time to check
  data testTemperatureData = getSensorData("temperature");
  data testHumidityData = getSensorData("humidity");

  if (testTemperatureData.success) {
    temperatureState = 1;
  }
  
  if (testHumidityData.success) {
    humidityState = 1;
  }

  return {
    temperatureState,
    humidityState,
    (temperatureState == 1) || (humidityState == 1),
  };
}

initData TemperatureAndHumiditySensor::getSensorState() {

  return {
    temperatureState,
    humidityState,
    (temperatureState == 1) || (humidityState == 1),
  };

}

data TemperatureAndHumiditySensor::getSensorData(String dataType) {

  float rawData;
  float minRange, maxRange;

  // Lower case the entry to avoid errors
  dataType = dataType.toLowerCase();

  if (dataType == "humidity") {

      if (humidityState == 0) {
        return {
          0.0,
          "Humidity sensor is in error.",
          false
        };
      }

      rawData = dht.readHumidity();
      minRange = humidityMinRange;
      maxRange = humidityMaxRange;
  } else if (dataType == "temperature") {

      if (temperatureState == 0) {
        return {
          0.0,
          "Temperature sensor is in error.",
          false
        };
      }

      rawData = dht.readTemperature();
      minRange = temperatureMinRange;
      maxRange = temperatureMaxRange;
  } else {
      return {
          0.0,
          "Please specify a data type to get.",
          false
      };
  }

  // Error verification
  if ((rawData < minRange) || (rawData > maxRange)) {
    return {
      0.0,
      "Data out of range",
      false
    };
  } else if (isnan(rawData)) {
    return {
      0.0,
      "Data not found",
      false
    };
  }

  return {
    rawData,
    "",
    true
  };

}