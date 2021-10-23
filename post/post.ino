#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

#define ssid "BEAR FAMILY | 2.4G"
#define password "bearhome"
#define serverName "http://192.168.1.6:9000/location"

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  
  Serial.println("[Connecting to WIFI]");
  while(WiFi.status() != WL_CONNECTED) {
  }
  Serial.println("[Connected to WiFi]");
  Serial.println("Delay 5 seconds before publishing the first reading.");
}

void loop() {
    //Check WiFi connection status
    if(WiFi.status() == WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
    
      http.begin(client, serverName);

      //Get package from LoRa Receiver and split
        String LoRa_Received_String = "";
        Serial.println("[Receiving data]");
        while (!Serial.available()) {
        }

        delay(100);
     
        int count = 0;
        String splited[4];
        String data_received = "";

        while (Serial.available()) {
          char temp_read = Serial.read();
           if (temp_read != 10)
             data_received += char(temp_read);
        }
      
        for(int i = 0; i < data_received.length(); i++) {
          char temp_read = data_received[i];
          if (temp_read != 10) {
            if (temp_read == 32)
              count++;
            else
              splited[count] += char(temp_read);
          }
        }

      //Send to web server
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST("{\"id\":\"" + splited[0] + "\",\"function\":\"" + splited[1] + "\",\"lat\":\"" + splited[2] + "\", \"long\":\"" + splited[3] + "\"}");
      
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
    }
}
