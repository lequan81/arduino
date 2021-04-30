#include <ESP8266WiFi.h> 
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"

#define DHTPIN 2     
int checksensor;

#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println("DHTxx test!");
  WiFi.begin("T2TECHHUB", "T2T@12345");
  while(WiFi.status() != WL_CONNECTED){
    Serial.println("..");
    delay(500);
    }
  dht.begin();
}

void loop() {
  delay(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  String request = "http://trungvan.heliohost.org/data.php?temp=" + String(t) + "&humi=" +String(h);
  Http_ReQuest(request);
  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
}


void Http_ReQuest(String request){
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    String url = request;
    http.begin(url);
    int httpCODE = http.GET();
       Serial.println(url);
       Serial.println("http code :"+String(httpCODE));
    if(httpCODE > 0){
      String payload = http.getString();
      Serial.println(payload);
      }
    else{
      Serial.printf("HTTP GET failed, ERRORS: %s\n", http.errorToString(httpCODE).c_str());
      }
      http.end();
    }
  }
