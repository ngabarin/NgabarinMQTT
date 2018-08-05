#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define LED_BUILTIN 2


// Informasi dasar tentang proyek dan nama sensor
#define PROJECT_NAME "tegar-imansyah"
#define NGABARIN_KEY "4a2a6374b83041b6e3a2d82d1656a5f783f7e554ab52a46a"
#define GATEWAY_NAME "semen-gresik"

// Options to connect to server
const char* mqttServer = "m14.cloudmqtt.com";
const int mqttPort = 17196;

char topic[50];
uint8_t x = sprintf(topic, "%s/data/%s", PROJECT_NAME, GATEWAY_NAME);

WiFiClient espClient;
PubSubClient client(espClient);

// WiFi Options
const char* ssid = "http://ngabarin.id";
const char* password =  "namadepanmu"; 

// For Multitasking
long previousMillis = 0; 
long interval = 8000; 


void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  pinMode(LED_BUILTIN, OUTPUT);

  while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(LED_BUILTIN, LOW);
      delay(250);
      Serial.printf("Connecting to WiFi %s...\n", ssid);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(250);
  }
    
  client.setServer(mqttServer, mqttPort);

}

void loop() {

  unsigned long currentMillis = millis();
  
  while (!client.connected()) {
    Serial.println(F("Connecting to MQTT..."));
    
    if (client.connect(GATEWAY_NAME, PROJECT_NAME, NGABARIN_KEY )) {

        Serial.println(F("connected"));
        digitalWrite(LED_BUILTIN, LOW);

    } else {

        Serial.printf("failed with state ");
        Serial.println(client.state());
        digitalWrite(LED_BUILTIN, HIGH);
        delay(2000);

    }
  }

  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;   
  
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& data = jsonBuffer.createObject();

    JsonArray& value = jsonBuffer.createArray();

    value.add(10);
    value.add(15);
    value.add(90);
  
    data["name"] = "name";
    data["value"] = value;
    
    char data_publish[200];
    data.printTo(data_publish);
    data.printTo(Serial);
    
    client.publish(topic, data_publish);
    
  }
}
