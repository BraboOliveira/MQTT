#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Adafruit_VL53L0X.h"
#include <WiFiManager.h> 

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

//Pinos Sensores
int sensorT = A0;
int valueT = 0;
int valueF = 0;
int botao = D5;

// Update these with values suitable for your network.

const char* ssid = "Rafael";
const char* password = "rafael2021";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value1 = 0;
int value2 = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    WiFiManager wifiManager;
    wifiManager.autoConnect("Sensor Pé Configurar");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereço de IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("Distancia", "conectadoConectado");
      // ... and resubscribe
//      client.subscribe("SensorT");
//      client.subscribe("SensorF");
    } else {
      Serial.print("failed, rc=");
//      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(500);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(botao, INPUT);
    while (! Serial) {
    delay(10);
  }
    Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
//    while(1);
  }
  // power 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 
}

void loop() {
    VL53L0X_RangingMeasurementData_t measure;
//  Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
//    Serial.print("Distancia (mm): "); Serial.println(measure.RangeMilliMeter);
  } else {
//    Serial.println(" out of range ");
  }

  valueT = analogRead(sensorT);
  valueF = digitalRead(botao);
//  Serial.print("Valor do botão:");
//  Serial.println(valueF);
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  unsigned long now = millis();
  if(now - lastMsg > 500){  
    if (valueT > 300) {
      lastMsg = now;
      ++value1;
      snprintf (msg, MSG_BUFFER_SIZE, "traseiro2 %ld", value1);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("Distancia", msg);
    while(valueT > 300) {
      valueT = analogRead(sensorT);
      delay(10);
     }
    }
    if (valueF) {
      lastMsg = now;
      ++value2;
      snprintf (msg, MSG_BUFFER_SIZE, "frontal2 %ld", value2);
      Serial.print("Mensagem Enviada: ");
      Serial.println(msg);
      client.publish("Distancia", msg);
    while(valueF) {
      valueF = digitalRead(botao);
      delay(10);
     }
    }
  }
}
