/*
    * Fetch BTC price from coinmarketcap API, and display on 8x7segment display.
    * 
    * code id based on Hector de Isidro worke: https://github.com/hrules6872
    * 
      TLSv1.2 is supported since version 2.4.0-rc1
    Created by Nimrod Galor, 2019.
    
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <LedControl.h>

#ifndef STASSID
#define STASSID "SSID"
#define STAPSK  "PASSWORD"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

const String API_KEY = "API_KEY";

const char* SYMBOL = "BTC";
const char* CONVERT_TO = "USD";

const long DEFAULT_DELAY = 60 * 1000 * 5; // wait 5 minutes between API request;

const char* host = "pro-api.coinmarketcap.com";
const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char fingerprint[] PROGMEM = "d3 79 2d dd ac 1f ed 90 8a a9 c1 c0 74 2d cf ff 6d 86 bf 0a";

// Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;

/* 
* Create a new controller 
* Params :*/
int dataPin = D5;    //The pin on the Arduino where data gets shifted out
int csPin = D6;      //The pin for selecting the device when data is to be sent
int clockPin = D7;   //The pin for the clock
int numDevices = 8; //The maximum number of devices that can be controlled

LedControl lc=LedControl(dataPin, clockPin, csPin, numDevices);

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

// init LedControl 
Serial.print("index: ");
Serial.println(lc.getDeviceCount());
  for(int index=0;index<lc.getDeviceCount();index++) {
      lc.shutdown(index,false);
      /* Set the brightness to a medium values */
      lc.setIntensity(index,8);
      /* and clear the display */
      lc.clearDisplay(index);
  }
}


void loop() {
  Serial.print("connecting to ");
  Serial.println(host);

  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  client.setFingerprint(fingerprint);

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = "/v1/cryptocurrency/quotes/latest?CMC_PRO_API_KEY=" + API_KEY + "&symbol=" + SYMBOL + "&convert=" + CONVERT_TO;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");

  // check HTTP response
  char httpStatus[32] = {0};
  client.readBytesUntil('\r', httpStatus, sizeof(httpStatus));
  if (strcmp(httpStatus, "HTTP/1.1 200 OK") != 0) {
    Serial.print("Unexpected response: ");
    Serial.println(httpStatus);
    delay();
    return;
  }

  
  // skip HTTP headers
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
    if (line == "\r") {
      break;
    }
  }

  // skip content length
  if (client.connected()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
  }

  // get response
  String response = "";
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
    line.trim();
    if (line != "\r") {
      response += line;
    }
  }

  client.stop();

  // parse response
  DynamicJsonDocument jsonDocument(1024);
  DeserializationError error = deserializeJson(jsonDocument, response);
  if (error) {
    Serial.println("Deserialization failed");
    delay();
    return;
  }
  JsonObject root = jsonDocument.as<JsonObject>();

  // check API status
  JsonObject status = root["status"];
  int statusErrorCode = status["error_code"];
  if (statusErrorCode != 0) {
    String statusErrorMessage = status["error_message"];
    Serial.print("Error: ");
    Serial.println(statusErrorMessage);
    delay();
  }

  JsonObject coin = root["data"][SYMBOL];
  String name = coin["name"];
  String lastUpdated = coin["last_updated"];
  Serial.println("Name: " + name);
  Serial.println("Last updated: " + lastUpdated);

  JsonObject quote = coin["quote"][CONVERT_TO];
  float price = quote["price"];
  Serial.print("Price: $");
  Serial.println(price, 2);

  printPrice((int)price);
  delay();




  
}

void printPrice(int price){
  // reset
  for(int index=0;index<lc.getDeviceCount();index++) {
    lc.clearDisplay(index);
  }

  int index = floor((8-sizeof(price)) /2);
  while(price > 0){
    lc.setDigit(0, index, price%10, false);
    price = price/10;
    index++;
  }
  
  
}
  
void delay() {
  Serial.println("");
  Serial.println("Keep calm and hodl!");
  delay(DEFAULT_DELAY);
}