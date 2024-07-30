#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
Adafruit_MPU6050 mpu;
#include "ThingSpeak.h"
float lat;
float lng;
const char* ssid = "Innovation Harsh";   // your network SSID (name) 
const char* password = "123Harshit";   // your network password
unsigned long myChannelNumber = 1;
const char * myWriteAPIKey = "77M0HH4J6DMW524V";
WiFiClient client;
// Timer variables
// Timer variables
int D_value, A_value;
unsigned long dataMillis = 0;
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;
#define MQ2_A 35 // define MQ2 analog pin
#define MQ2_D 14 // define MQ2 digital pin
#define LED 25
TinyGPSPlus gps;
SoftwareSerial gpsSerial(3, 1); // RX, TX
char buffer[100];

void printData() 
{
    if (gps.location.isUpdated()) {
         lat = gps.location.lat();
         lng = gps.location.lng();

        double altitude = gps.altitude.meters();

        int year = gps.date.year();
        int month = gps.date.month();
        int day = gps.date.day();

        int hour = gps.time.hour();
        int minute = gps.time.minute();
        int second = gps.time.second();
sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
        snprintf(buffer, sizeof(buffer),
                 "Latitude: %.8f, Longitude: %.8f, Altitude: %.2f m, "
                 "Date/Time: %d-%02d-%02d %02d:%02d:%02d",
                 lat, lng, altitude, year, month, day, hour, minute, second);

        Serial.println(buffer);
    }
}
void setup(void) {
  Serial.begin(115200);
  pinMode(MQ2_A, INPUT);
  pinMode(MQ2_D, INPUT);
    gpsSerial.begin(9600);
   WiFi.mode(WIFI_STA);  
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);
}

void loop() {
  
   if ((millis() - lastTime) > timerDelay) {
    
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
     // while(WiFi.status() != WL_CONNECTED)
     // {
        WiFi.begin(ssid, password); 
        delay(5000);     
     // } 
      Serial.println("\nConnected.");
    }
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");

  Serial.println("");
  delay(500);
  //A_value = analogRead(MQ2_D);
  D_value = digitalRead(MQ2_D);

  Serial.printf("Analog value: %d\n", A_value);
  Serial.printf("Digital value: %d\n", D_value);
   ThingSpeak.setField(1, a.acceleration.x);
      ThingSpeak.setField(2, a.acceleration.y);
      ThingSpeak.setField(3, a.acceleration.z);
      ThingSpeak.setField(4, temp.temperature);
      ThingSpeak.setField(5, lat);
      ThingSpeak.setField(6, lng);
      ThingSpeak.setField(7,  D_value);
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
  }

  if (gpsSerial.available() > 0) {
  if (gps.encode(gpsSerial.read())) {
             printData();
         }
     }


  // A_value = analogRead(MQ2_D);
  // D_value = digitalRead(MQ2_D);

  // Serial.printf("Analog value: %d\n", A_value);
  // Serial.printf("Digital value: %d\n", D_value);

  // ThingSpeak.setField(1, A_value);
  // ThingSpeak.setField(2, !D_value);

  // if (millis() - dataMillis > 20000)
  // {
  //   int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  //   if(x == 200){
  //     Serial.println("Channel update successful.");
  //   }
  //   else {
  //     Serial.println("Problem updating channel. HTTP error code " + String(x));
  //   }  
  // }

  if(!D_value){
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
  }
}