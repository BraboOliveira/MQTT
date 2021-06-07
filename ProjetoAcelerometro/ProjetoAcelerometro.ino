#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <DNSServer.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

unsigned long Envio = millis();

#include <PubSubClient.h>
#include "Adafruit_VL53L0X.h"
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
//Pinos Sensores
int sensorT = A0;
int valueT = 0;
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msgMqtt[MSG_BUFFER_SIZE];
int value = 0;
const char* mqtt_server = "broker.hivemq.com";


// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"

#include "MPU6050_6Axis_MotionApps20.h"
//#include "MPU6050.h" // not necessary if using MotionApps include file

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;
//MPU6050 mpu(0x69); // <-- use for AD0 high


// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector


// uncomment "OUTPUT_TEAPOT_OSC" if you want output that matches the
// format used for the InvenSense teapot demo
#define OUTPUT_TEAPOT_OSC


#ifdef OUTPUT_READABLE_EULER
float euler[3];         // [psi, theta, phi]    Euler angle container
#endif
#ifdef OUTPUT_READABLE_YAWPITCHROLL
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
#endif

#define INTERRUPT_PIN 15 // use pin 15 on ESP8266

const char DEVICE_NAME[] = "mpu6050";

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(10, 0, 0, 107);  // remote IP to receive OSC
const unsigned int outPort = 9999;          // remote port to receive OSC

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
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
      client.publish("SensorPe", "Conectado");
      // ... and resubscribe
      client.subscribe("SensorPe");
      client.subscribe("Distancia");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(500);
    }
  }
}

void mpu_setup()
{
  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  // initialize device
  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();
  pinMode(INTERRUPT_PIN, INPUT);

  // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

  // load and configure the DMP
  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    Serial.println(F("DMP ready! Waiting for first interrupt..."));
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    Serial.print(F("DMP Initialization failed (code "));
    Serial.print(devStatus);
    Serial.println(F(")"));
  }
}

void setup(void)
{
  Serial.begin(250000);
  Serial.println(F("\nOrientation Sensor OSC output")); Serial.println();

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset saved settings
  //wifiManager.resetSettings();

  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect(DEVICE_NAME);

  Serial.print(F("WiFi connected! IP address: "));
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
  }
  mpu_setup();
}
void readAnalogSensor() {
  valueT = analogRead(sensorT);
  //  Serial.println("Lendo");
}
void mpu_loop()
{
  mpuInterrupt = true;
  // if programming failed, don't try to do anything
  if (!dmpReady) return;

  // wait for MPU interrupt or extra packet(s) available
  if (!mpuInterrupt && fifoCount < packetSize) return;

  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

  // get current FIFO count
  fifoCount = mpu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    // reset so we can continue cleanly
    mpu.resetFIFO();
    Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (1) {
    // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    // track FIFO count here in case there is > 1 packet available
    // (this lets us immediately read more without waiting for an interrupt)
    fifoCount -= packetSize;

#ifdef OUTPUT_TEAPOT_OSC
#ifndef OUTPUT_READABLE_QUATERNION
    // display quaternion values in easy matrix form: w x y z
    mpu.dmpGetQuaternion(&q, fifoBuffer);
#endif
    // Send OSC message
    OSCMessage msg("/imuquat");
    Serial.print("\tA");
    Serial.print((float)q.w);
    Serial.print("\tX");
    Serial.print((float)q.x);
    Serial.print("\tY");
    Serial.print((float)q.y);
    Serial.print("\tZ");
    Serial.println((float)q.z);
    msg.add((float)q.w);
    msg.add((float)q.x);
    msg.add((float)q.y);
    msg.add((float)q.z);
    Udp.beginPacket(outIp, outPort);
    if ((millis()- Envio)> 1000) {
      Serial.println("Enviou");
      snprintf (msgMqtt, MSG_BUFFER_SIZE, "Angulo: %.2f", (float)q.w);
      client.publish("Angulo", msgMqtt);
      snprintf (msgMqtt, MSG_BUFFER_SIZE, "EixoX: %.2f", (float)q.x);
      client.publish("EixoX", msgMqtt);
      snprintf (msgMqtt, MSG_BUFFER_SIZE, "EixoY: %.2f", (float)q.y);
      client.publish("EixoY", msgMqtt);
      snprintf (msgMqtt, MSG_BUFFER_SIZE, "EixoZ: %.2f", (float)q.z);
      client.publish("EixoZ", msgMqtt);
      Envio = millis();
    }
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();

#endif
  }
}

/**************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************/
void loop(void)
{
  dmpDataReady();
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println();
    Serial.println("* Disconnected from AP so rebooting *");
    Serial.println();
    ESP.reset();
  }
  client.loop();
  if (!client.connected()) {
    reconnect();
  }
  mpu_loop();
}
