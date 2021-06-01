#include <SPISlave.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define __packed__ __attribute__ ((packed))

/*
GPIO    NodeMCU   Name  |   Uno
   ===================================
     15       D8       SS   |   D10
     13       D7      MOSI  |   D11
     12       D6      MISO  |   D12
     14       D5      SCK   |   D13


     */

ESP8266WebServer webServ(80);

#define C_BUFFWRITE 0x01

#define BUFSIZE (174*144/2*2)

uint8_t buff[BUFSIZE];

void retrieveData() {
  webServ.send(200, "application/octet-stream", buff, BUFSIZE);
}

int v = 0;

void handleSpiData(uint8_t *data, size_t len) {
  //len = 32
  switch (data[0]) {
    case 0x01:
    Serial.print("Image start");
  }

  Serial.println("Received data");
  for (int i = 0; i < len; i++) {
    Serial.print(data[i]);
    Serial.print(" ");
  }
  Serial.println("Set status ");
 Serial.print(v);
  SPISlave.setStatus(v++);  
}

void setup() {
  // put your setup code here, to run once
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

  webServ.on("/", HTTP_GET, retrieveData);
  webServ.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  webServ.handleClient();
}
