// dust_humid
#include "DHT.h"

// gps
#include <SD.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <stdlib.h> // float to string 

//dust_humid
int measurePin = 0;
int ledPower = 8;

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured = 0;  
float calcVoltage = 0;
float dustDensity = 0;


// INIT DHT sensor 
#define DHTPIN 2
#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);

//gps
static const int RXPin = 4;
static const int TXPin = 3;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

// indicator leds
static const int GPS_OK_PIN = 6;
static const int GPS_NOT_OK_PIN = 7;

// SD card things
int CS_PIN = 10;
File file;


void setup(){
  pinMode(ledPower,OUTPUT);
  ss.begin(9600);
   
  // init indicator leds
  pinMode(GPS_OK_PIN, OUTPUT);
  pinMode(GPS_NOT_OK_PIN, OUTPUT);
   
  digitalWrite(GPS_OK_PIN, HIGH);
  digitalWrite(GPS_NOT_OK_PIN, HIGH);
   


  // init SD card
  pinMode(CS_PIN, OUTPUT);
  SD.begin();
  char filename[] = "data.csv";

  // check for file. if exisist append to it, else 
  // open & write csv header 
  if(SD.exists(filename)){
    file.close(); 
    file = SD.open(filename, O_WRITE | O_APPEND); 
  }
  else{
    file = SD.open(filename, O_WRITE | O_CREAT); 
    file.print("Time, Latitude, Longitude, Dust(ug/m3), Humidity, Temp (C)\n"); 
    file.flush();
  } 

  dht.begin();

}

void loop() {

  while (ss.available() > 0) { 
     if (gps.encode(ss.read())){
       if(gps.location.isValid()){
         digitalWrite(GPS_OK_PIN, HIGH);
         digitalWrite(GPS_NOT_OK_PIN, LOW);
         writeToFile();
         delay(4000);
       } else {
         digitalWrite(GPS_OK_PIN, LOW);
         digitalWrite(GPS_NOT_OK_PIN, HIGH);
       }
     }
  }
}

void writeToFile(){

  //gps
  file.print(gps.date.day());
   file.print("/");
   file.print(gps.date.month());
   file.print("/");
   file.print(gps.date.year());
   file.print(" ");
   file.print(gps.time.hour());
   file.print(":");
   file.print(gps.time.minute());
   file.print(":");
   file.print(gps.time.second());
   file.print(","); 
   
   // lat
   char lat[15];
   dtostrf(gps.location.lat(), 3, 6, lat);
   
   file.print(lat);
   file.print(",");
   
   // long
   char lng[15];
   dtostrf(gps.location.lng(), 3, 6, lng);
   file.print(lng);
   file.print(",");
   
  //dust
  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(samplingTime);
  voMeasured = analogRead(measurePin); // read the dust value
  
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);

  calcVoltage = voMeasured * (5.0 / 1024);
  // linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
  // Chris Nafis (c) 2012
  dustDensity = (0.17 * calcVoltage - 0.0999999) * 1000;
  if (dustDensity < 0) {
    dustDensity = 0;
  }
  file.print(dustDensity);
  file.print(",");


  // humid
  file.print(dht.readHumidity());
  file.print(",");

  //temp
  file.print(dht.readTemperature());
  file.print("\n");

  //ensure that data is written
  file.flush();
}

