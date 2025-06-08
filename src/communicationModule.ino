// This file ensure and handle all communications to the ground.

struct outboundMessage {
  bool isSensorMessage;
  float sensorValue;
  String sensorType;
  bool sensorSuccess;
  String textMessage;
  unsigned long timestamp;
  unsigned long id;   
};

class CommunicationModule
{
  public:
    bool initCommunications();
    void readInbound();
    bool sendToGround(outboundMessage message);
    bool healthCheck();

  private:
    String formatMessageAsJSON(outboundMessage message);
};


bool CommunicationModule::initCommunications() {

  // We used 9600 to reduce the interferences between our systems and his environment.
  Serial.begin(9600);

  // Wait 10 seconds for the Serial to be totally ready.
  unsigned long startTime = millis();
  while (!Serial && (millis() - startTime < 10000)) {
    delay(10); // To avoid overloading the CPU
  }

  // If the serial isn't ready after the timeout, we have a problem with the communication
  if (!Serial) {
    return false;
  }

  // We clear all buffers to start on clean base
  while (Serial.available()) {
    Serial.read();
  }

  // Try a first healthcheck
  bool healthcheck = healthCheck();

  return healthcheck;

};

void CommunicationModule::readInbound() {

  if (Serial.available()) {
    String incomingMessage = Serial.readStringUntil('\n');
    incomingMessage.trim(); // Remove white-space before and after

    Serial.print("Inboud message from the ground: "); 
    Serial.println(incomingMessage);

  }

}

bool CommunicationModule::sendToGround(outboundMessage message) {

  if (!Serial.availableForWrite()) {
    return false;
  }

  // Forlmat the message to send it correctly
  String jsonMessage = formatMessageAsJSON(message);

  Serial.println(jsonMessage);

  return true;

}

bool CommunicationModule::healthCheck() {

  // If the serial isn't ready anymore we return false
  if (!Serial) {
    return false;
  }

  // Clean the buffer before the healthcheck
  while (Serial.available()) {
    Serial.read();
  }

  Serial.println("ping");

  String response = "";
  unsigned long startTime = millis();
  
  // Wait a maximum of 15 seconds to receive a response.
  while ((millis() - startTime < 15000)) {
    if (Serial.available()) {
      char receivedChar = Serial.read();
      
      // Build the response and stop when a new line is detected
      if (receivedChar != '\n' && receivedChar != '\r') {
        response += receivedChar;
      } else {
        break;
      }
    }
  }

  // Delete white-space before and after the response
  response.trim();
  return (response == "pong");
}

String CommunicationModule::formatMessageAsJSON(outboundMessage message) {
  String json = "{";
  
  // Add timestamp and ID (always present in a message)
  json += "\"timestamp\":" + String(message.timestamp) + ",";
  json += "\"id\":" + String(message.id) + ",";
  
  if (message.isSensorMessage) {
    json += "\"type\":\"sensor_data\",";
    json += "\"sensor_type\":\"" + message.sensorType + "\",";
    json += "\"value\":" + String(message.sensorValue) + ",";
    json += "\"success\":" + (message.sensorSuccess ? "true" : "false");
  } else {
    json += "\"type\":\"text_message\",";
    json += "\"message\":\"" + message.textMessage + "\"";
  }
  
  json += "}";
  return json;
}