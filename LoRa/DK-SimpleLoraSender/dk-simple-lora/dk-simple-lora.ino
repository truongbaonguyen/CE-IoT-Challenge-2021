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

void setup() {
  Serial.begin(9600);

  pinMode(LS_LED_BLUE, OUTPUT);

  pinMode(LS_GPS_ENABLE, OUTPUT);
  digitalWrite(LS_GPS_ENABLE, LOW);
  pinMode(LS_GPS_V_BCKP, OUTPUT);
  digitalWrite(LS_GPS_V_BCKP, LOW);

  while (!Serial && millis() < 3000);
  
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
  
  Serial.println();
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

  // DATA FROM SENSOR SIMULATION
  
  Serial.print("[Getting GPS data] ");
  while (Serial.available() <= 0) {
    if (myGPS_data != "")
      break;
  }

  Serial.print("GPS data: ");
  if (Serial.available() > 0) {
    myGPS_data = "";
    while (Serial.available() > 0) {
      char temp_read = Serial.read();
      if (temp_read != 10)
        myGPS_data += char(temp_read);
    }
  }

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
