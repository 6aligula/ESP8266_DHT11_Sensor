#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DHTPIN 2
//definir tipo de dispositivo (DHT11)
#define DHTTYPE DHT11
const char* ssid = "";
const char* password = "";
const char* mqtt_server = "";

const char* keyDevice = "DHTMODULO1";
//const char* dht11;
//int devicePin = 2;
DHT dht11(DHTPIN, DHTTYPE);
//DHT dht11(devicePin); 

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
float respaldo_temp = 25;
float temperatura;
float respaldo_humedad = 45;
float humedad;
int contador;

void setup() {
  pinMode(DHTPIN, INPUT);
  setup_wifi();
  dht11.begin();
  delay(2000);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);  
}

void setup_wifi() {

  delay(10);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

}

void reconnect() {

  while (!client.connected()) {
    if (client.connect("dht11modulo")) {
      client.publish("test", keyDevice);
      client.subscribe(keyDevice);
    } else {
      delay(5000);
    }
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (!client.connected()) {
    reconnect();
  } 

  client.loop();

  if (currentMillis - lastMsg > 60000) {  // 900000 ms = 15 minutos
    lastMsg = currentMillis;

    // Leyendo temperatura y humedad
    temperatura = dht11.readTemperature(false, true);
    humedad = dht11.readHumidity(true);

    // Controlando fallos de lectura de temperatura y humedad del sensor
    if (!isnan(temperatura)) {
      respaldo_temp = temperatura;
    }

    if (!isnan(humedad)) {
      respaldo_humedad = humedad;
    }

    // Publicando temperatura y humedad
    client.publish("casa/lab/temperatura", String(respaldo_temp).c_str());
    client.publish("casa/lab/humedad", String(respaldo_humedad).c_str());
  }
}
