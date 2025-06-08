// This file handle all other modules and orchestrate the startup of the systems and its health.

#include "TemperatureAndHumiditySensor.h"
#include "CommunicationModule.h"

TemperatureAndHumiditySensor sensor;
CommunicationModule comm;

bool commInit = false;
bool sensorInit = false;
bool temperatureSensorState = 0;
bool humiditySensorState = 0;

unsigned long messageId = 0;
unsigned long startTime;
unsigned long lastMessageSent;
unsigned long lastHealthcheck;

struct data {
  float value;
  String errorMessage;
  bool success;
};

void setup() {
  // Init the communication module at first, we try five time if it's won't launch at the first time
  int commTry = 1;
  while (!commInit && (commTry < 5)) {
    commTry++;
    commInit = comm.initCommunications();
    delay(10); // To avoid overloading the CPU
  }

  // Init the sensor module, we try five time if it's won't launch at the first time
  int sensorTry = 1;
  while (!sensorInit && (sensorTry < 5)) {
    sensorTry++;
    sensorInit = sensor.initSensor().success;
    delay(10); // To avoid overloading the CPU
  }

  if (sensorInit) {

    temperatureSensorState = sensor.getSensorState().temperatureSensorUp;
    humiditySensorState = sensor.getSensorState().HumiditySensorUp;

  }

  startTime = millis();
  lastMessageSent = millis();
  lastHealthcheck = millis();
}

void loop() {

  // We send sensor data every 30 seconds
  if (millis() - lastMessageSent > 30000) {

    unsigned long currentTimestamp = getTimestamp();

    if (temperatureSensorState == 1) {

      data temperature = sensor.getSensorData("temperature");

      if (temperature.success) {
        bool temperatureCommSent = comm.sendToGround({
          true,
          temperature.value,
          "temperature",
          temperature.success,
          currentTimestamp,
          messageId,
        });

        if (temperatureCommSent) {
          messageId++;
        }
      }

    }

    if (humiditySensorState == 1) {

      data humidity = sensor.getSensorData("humidity");

      if (humidity.success) {
        bool humidityCommSent = comm.sendToGround({
          true,
          humidity.value,
          "humidity",
          humidity.success,
          currentTimestamp,
          messageId,
        });

        if (humidityCommSent) {
          messageId++;
        }
      }

    }

    lastMessageSent = millis();

  } else if (millis() - lastHealthcheck > 15000) {
    // Proceed to healthcheck between every sensor message, not at the same time.

    if (commInit) {

      bool healthy = comm.healthCheck();

      if (!healthy) {
        // What we do here ?
      }

    }

    lastHealthcheck = millis();

  }

  if (commInit) {

    comm.readInbound();

  }

}

unsigned long getTimestamp() {

  return (millis() - startTime);

}