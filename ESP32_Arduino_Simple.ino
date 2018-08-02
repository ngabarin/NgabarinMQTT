// Program ini berjalan untuk ESP32 DEVKIT V1 - DOIT
// Untuk board lain, kemungkinan besar dapat berjalan namun perlu penyesuaian LED_BUILTIN

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define LED_BUILTIN 2

// Informasi dasar tentang proyek dan nama sensor
#define PROJECT_NAME "airpolish"
#define NGABARIN_KEY "165ee34847e5cd8cb5768d93a3e3f12afb1d75e04c4833bf"
#define GATEWAY_NAME "semen-gresik"

// Informasi server yang terhubung dengan ngabarin
const char* mqttServer = "m14.cloudmqtt.com";
const int mqttPort = 17196;

char topic[50];
uint8_t x = sprintf(topic, "%s/data/%s", PROJECT_NAME, GATEWAY_NAME);

// Objek WiFi dan MQTT Client
WiFiClient espClient;
PubSubClient client(espClient);

// Informasi WiFi yang ingin dihubungkan
const char* ssid = "http://ngabarin.id";
const char* password =  "namadepanmu"; 

// Variabel untuk mengirim data setiap 8 detik
long previousMillis = 0; 
long interval = 8000; 


void setup() {
  // Mengaktifkan Serial di baudrate 115200
  Serial.begin(115200);
  
  // Menghubungkan dengan WiFi
  // Menyalakan indikator LED_BUILTIN untuk informasi terhubung
  WiFi.begin(ssid, password);
  pinMode(LED_BUILTIN, OUTPUT);

  while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(LED_BUILTIN, LOW);
      delay(250);
      Serial.printf("Connecting to WiFi %s...\n", ssid);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(250);
  }
  
  // Inisiasi server ngabarin
  client.setServer(mqttServer, mqttPort);

}

void loop() {
  // Melakukan pengecekan waktu saat ini
  unsigned long currentMillis = millis();
  
  // Melakukan pengecekan koneksi dengan server
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

  // Melakukan pengecekan apakah sudah 8 detik atau belum
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;   
  
    // Membuat objek JSON bernama data
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& data = jsonBuffer.createObject();
    
    /* Membentuk JSON
    {
      "name": "sensor",
      "value": 10
    }
    */
    
    data["name"] = "sensor";
    data["value"] = 10;
    
    // Convert JSON ke char
    char data_publish[200];
    data.printTo(data_publish);
    
    // Kirim ke server
    client.publish(topic, data_publish);
  }
}
