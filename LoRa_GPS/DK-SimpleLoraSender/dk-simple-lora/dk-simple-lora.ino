/*
 *  _                                   ____                       
 * | |    __ _  ___ _   _ _ __   __ _  / ___| _ __   __ _  ___ ___ 
 * | |   / _` |/ __| | | | '_ \ / _` | \___ \| '_ \ / _` |/ __/ _ \
 * | |__| (_| | (__| |_| | | | | (_| |  ___) | |_) | (_| | (_|  __/
 * |_____\__,_|\___|\__,_|_| |_|\__,_| |____/| .__/ \__,_|\___\___|
 *                                           |_|                   
 * Copyright (C) 2019 Lacuna Space Ltd.
 *
 * Description: Simple LoRaWAN example sketch
 * 
 * License: Revised BSD License, see LICENSE-LACUNA.TXT file included in the project
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef REGION
#define REGION R_EU868
#endif

// Interval between transmissions
#define TX_INTERVAL 120

#include <LibLacuna.h>
#include <SPI.h>
#include <Wire.h>
#include <RTC.h>
#include <time.h>
#include <MicroNMEA.h>   // http://librarymanager/All#MicroNMEA



// Keys and device address are MSB
static byte networkKey[] = { 
    // Replace with your network key
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static byte appKey[] = {
    // Replace with your application key
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
// Replace with your device address
static byte deviceAddress[] = { 0x01, 0x02, 0x03, 0x04 };

static char payload[255];

//

static lsLoraTxParams txParams;

float gnss_lat;                         // Global node postition lattiude
float gnss_lon;                         // Global node postition longitude
uint8_t GPS_Address = 0x10;

char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));

void setup() {
  Serial.begin(9600);

  pinMode(LS_GPS_ENABLE, OUTPUT);
  digitalWrite(LS_GPS_ENABLE, LOW); // Off
  delay(2000); // wait for GPS reset

  pinMode(LS_GPS_V_BCKP, OUTPUT);
  digitalWrite(LS_GPS_V_BCKP, HIGH);

  digitalWrite(LS_VERSION_ENABLE, LOW);

  pinMode(LS_INT_MAG, OUTPUT); // make INT_MAG LOW for low-power
  digitalWrite(LS_INT_MAG, LOW);
  Wire.begin();

  while (!Serial && millis() < 3000);

  pinMode(LS_LED_BLUE, OUTPUT);
  
  Serial.println("Initializing");

  Serial.print("Configured Region: ");
  #if REGION == R_EU868
    Serial.println("Europe 862-870 Mhz");
  #elif REGION  == R_US915
    Serial.println("US 902-928 Mhz");
  #elif REGION == R_AS923
    Serial.println("Asia 923 Mhz");
  #elif REGION == R_IN865
    Serial.println("India 865-867 Mhz");
  #else 
    Serial.println("Undefined");  
  #endif

  // SX1262 configuration for lacuna LS200 board
  lsSX126xConfig cfg;
  lsCreateDefaultSX126xConfig(&cfg, BOARD_VERSION);

  // Initialize SX1262
  int result = lsInitSX126x(&cfg, REGION);
  Serial.print("E22/SX1262: ");
  Serial.println(lsErrorToString(result));

  // transmission parameters for terrestrial LoRa
  lsCreateDefaultLoraTxParams(&txParams, REGION);
  txParams.frequency = 922000000;
  txParams.spreadingFactor = lsLoraSpreadingFactor_7;
  txParams.codingRate = lsLoraCodingRate_4_6;
  txParams.bandwidth = lsLoraBandwidth_500_khz;

  Serial.print("Terrestrial Uplink Frequency: ");
  Serial.println(txParams.frequency/1e6);
}

String myGPS_data = "";
String function_data = "";
String device_id = "";

void loop() {

  digitalWrite(LS_LED_BLUE, HIGH);
  delay(50);
  digitalWrite(LS_LED_BLUE, LOW);
  delay(100);
  digitalWrite(LS_LED_BLUE, HIGH);
  delay(50);
  digitalWrite(LS_LED_BLUE, LOW);

  // DETECT DEVICE ID

  Serial.println();
  Serial.print("[Detecting DEVICE ID] ");
  while (Serial.available() <= 0) {
    if (device_id != "")
      break;
  }

  Serial.print("Device ID: ");
  if (Serial.available() > 0 && device_id == "") {
    while (Serial.available() > 0) {
      char temp_read = Serial.read();
      if (temp_read != 10)
        device_id += char(temp_read);
    }
  }
  
  Serial.println(device_id);


  // DETECT SOS FUNCTION
  
  Serial.print("[Detecting SOS Function] ");
  while (Serial.available() <= 0) {
    if (function_data != "")
      break;
  }

  Serial.print("Function: ");
  if (Serial.available() > 0 && function_data == "") {
    while (Serial.available() > 0) {
      char temp_read = Serial.read();
      if (temp_read != 10)
        function_data += char(temp_read);
    }
  }
  
  Serial.println(function_data);

  // Update GPS

  Serial.print("[Getting GPS data] ");
  updategps();

  // DATA FROM SENSOR SIMULATION
  
  Serial.print("GPS data: ");
  myGPS_data = "";
  myGPS_data = String(gnss_lat,6) + " " + String(gnss_lon,6);
  Serial.println(myGPS_data);

  // SEND DATA VIA LORA

  String all_data = device_id + " " + function_data + " " + myGPS_data;
  Serial.print("[Data] ");
  Serial.println(all_data);
  
  Serial.print("[Sending data via LoRa] "); 
  all_data.toCharArray(payload, 255); 
  int lora_result  = lsSendLora(&txParams, (byte *)payload, all_data.length(), false);
  Serial.print("Result: ");
  Serial.println(lsErrorToString(lora_result));
 
  // wait TX_INTERVAL seconds
  delay(TX_INTERVAL*1000);

}

byte updategps() {

// Switch on GPS
   digitalWrite(LS_GPS_ENABLE, HIGH);
   delay(200);
   
   while (!nmea.isValid())
   {   
     
  // Get NMEA data from I2C
  Wire.requestFrom(GPS_Address, 255);
  while (Wire.available()) {
    char c = Wire.read();
    nmea.process(c);
    //Serial.print(c);
     }
    delay(2000);
   }


//If a message is recieved print all the informations
   if (nmea.isValid())
   {   
      gnss_lat = nmea.getLatitude()/1.0e6;
      gnss_lon = nmea.getLongitude()/1.0e6;      
      return(1); //exit function
    }
  return(0);  
}
