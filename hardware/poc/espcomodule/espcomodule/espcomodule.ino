#include <SPISlave.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include<StreamDev.h>

#define __packed__ __attribute__ ((packed))

/*
GPIO    NodeMCU   Name  |   Uno
   ===================================
     15       D8       SS   |   D10
     13       D7      MOSI  |   D11
     12       D6      MISO  |   D12
     14       D5      SCK   |   D13


     */

#define busyPin 5 /* GPIO5 (D1) for LED */


#define C_BUFFWRITE 0x01

 uint8_t buff[32];
volatile int be = 0;

volatile int shouldSend = 0;

WiFiClient *client;

void handleSpiData(uint8_t *data, size_t len) {
  //len = 32
  digitalWrite(busyPin, LOW);
  switch (data[0]) {
    case 0x01: {
    if (shouldSend != 0) {
      Serial.println("x");  
      digitalWrite(busyPin, HIGH);    
      return;
    }
    Serial.println("Image start req");
    shouldSend = 1;
    break;
    }

    case 0x02:
    if (!shouldSend) {
     // Serial.println("Write outside buff");
      digitalWrite(busyPin, HIGH);
      return;
    }
    //Serial.print(".");
    memcpy(buff, data+1, 31);
    be = 1;
    break;

    default:
    Serial.println("Unrecognized CMD");
    Serial.println(data[0]);
    digitalWrite(busyPin, HIGH);
  }
/*
  Serial.println("Received data");
  for (int i = 0; i < len; i++) {
    Serial.print(data[i]);
    Serial.print(" ");
  }
  Serial.println("Set status ");
  SPISlave.setStatus(0);  
  */
}

void setup() {
  // put your setup code here, to run once
  pinMode(busyPin, OUTPUT);
  digitalWrite(busyPin, LOW);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin("Home24.1_2.4G", "AbCdEfGh");

  SPISlave.onData(handleSpiData);
  SPISlave.begin();
  
  Serial.println("CWSYS COMODULE Starting up");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("Connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
   
/*
  HTTPClient http;
  http.begin(*client, "http://73ae86b7fb60.ngrok.io/");
  Serial.println("GET ...");
  Serial.println(http.GET());
  http.end();
  */
}

#define URL "http://192.168.1.147:8080"

/*
void httpSend() {
  Serial.println("Start connection");
  HTTPClient http;
  WiFiClient client;
  http.begin(client, URL);
  http.addHeader("Content-Type", "application/octet-stream");
  if(!http.sendRequest("POST", &istream, 50112)) {
    Serial.println("Send error");  
  }
  http.end();
   Serial.println("Send DONE");  
}*/

void loop() {
  client = new WiFiClient();
  digitalWrite(busyPin, HIGH);
  while (shouldSend == 0) delay(100);
  //Make connection
  Serial.println("HTTP Connecting");
  if (!client->connect("35.223.209.219", 80)) {
      Serial.println("HTTP Connection failed");  
      shouldSend = 0;
      delete client;
      return;
    }
    
    
    Serial.println("HTTP Connection established");  
    const char *header = "POST /footage HTTP/1.1\r\nHost: 35.223.209.219\r\nConnection: close\r\nContent-Length: 50112\r\nContent-Type: application/octet-stream\r\n\r\n";
    int l = client->write(header, strlen(header));
    Serial.println(l);
    Serial.println(" expect ");  
    Serial.println(strlen(header));

  Serial.println("Phase 2: Data trans");  

  int ttl = 50112;

  uint8_t ordt = 0;
  int sz = 0;
  while (ttl) {
    //Serial.println(brem);
    
    be = 0;
    digitalWrite(busyPin, HIGH);
    while (be == 0) delay(1);

    if (buff[0] != (buff[1] ^ 0xAA))
      continue;
      
    while (ordt != buff[0]) {
      int l = ttl < 12 ? ttl : 12;
      client->write(buff+2, l);
      sz += l;
      ordt++;
      ttl -= l;
    }
    int l = ttl < 12 ? ttl : 12.;
      client->write(buff+2, l);
      sz += l;
      ordt++;
      ttl -= l;

    if (sz % 500 == 0) {
      Serial.println("Progress: ");
      Serial.print(sz);
      Serial.println("/50112");
    }
  }

  Serial.println("Write total=");
  Serial.print(ttl);
    
      Serial.println("HTTP Done");
      Serial.println("Check header");

      while (client->connected()) {
        while (client->available() > 0) {
        String s = client->readStringUntil('\n');
        int ppl;
        
        Serial.println(s);  
        if (sscanf(s.c_str(), " {\"peopleCount\":%d", &ppl) == 1) {
          Serial.println("Detect PCNT: ");
          Serial.print(ppl);
          SPISlave.setStatus(ppl);
        }
        }
        
        delay(0);
      }
      Serial.println("Flush");
      while (client->available() > 0) {
        while (client->available() > 0) {
        String s = client->readStringUntil('\n');
        int ppl;
        
        Serial.println(s);  
        if (sscanf(s.c_str(), " {\"peopleCount\":%d", &ppl) == 1) {
          Serial.println("Detect PCNT: ");
          Serial.print(ppl);
          SPISlave.setStatus(ppl);
        }
        } 
      }
      client->stop();
      
      digitalWrite(busyPin, LOW);
    delete client;
    shouldSend = 0;
    Serial.println("OK");
}
