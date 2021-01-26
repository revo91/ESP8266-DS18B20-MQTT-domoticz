#include <ESP8266WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define WIFISSID "" // Put your WifiSSID here
#define PASSWORD "" // Put your wifi password here
#define TOKEN "" // Put your Ubidots' TOKEN
#define MQTT_CLIENT_NAME "" // MQTT client Name, please enter your own 8-12 alphanumeric character ASCII string; 
                                           //it should be a random and unique ascii string and different from all other devices

// GPIO where the DS18B20 is connected to
const int oneWireBus = 14;     

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

char mqttBroker[]  = "192.168.8.250"; //Put your MQTT broker address
char payload[100];
char topic[150];
char topicSubscribe[100];

/****************************************
 * Auxiliar Functions
 ****************************************/
WiFiClient ubidots;
PubSubClient client(ubidots);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Attemp to connect
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {

      Serial.println("Connected");
      client.subscribe(topicSubscribe);

    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}

/****************************************
 * Main Functions
 ****************************************/
void setup() {
  Serial.begin(115200);
  sensors.begin();
  WiFi.begin(WIFISSID, PASSWORD);
  
  Serial.println();
  Serial.print("Wait for WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttBroker, 1883);
}

void loop() {
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.println(temperatureC);
  
  if (!client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {   
    reconnect();
  } else {
    sprintf(payload, "{\"idx\":8,\"nvalue\":0,\"svalue\":\"%f\"}", temperatureC); 
      client.publish("domoticz/in", payload);
  }
  
  client.loop();
  delay(10000);
}