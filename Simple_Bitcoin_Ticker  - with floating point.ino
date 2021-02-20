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
#define STASSID ""
#define STAPSK  ""
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

const String API_KEY = "";

const char* CONVERT_TO = "USD";

const char* SYMBOL = "BTC";
//const char* SYMBOL = "ETH";//ethereum
//const char* SYMBOL = "XRP";//ripple
//const char* SYMBOL = "BCH";//bitcoin-cash
//const char* SYMBOL = "LTC";//litecoin
//const char* SYMBOL = "EOS";//eos
//const char* SYMBOL = "USDT";//tether
//const char* SYMBOL = "BNB";//binance-coin
//const char* SYMBOL = "XLM";//stellar
//const char* SYMBOL = "ADA";//cardano
//const char* SYMBOL = "TRX";//tron
//const char* SYMBOL = "XMR";//monero
//const char* SYMBOL = "BSV";//bitcoin-sv
//const char* SYMBOL = "DASH";//dash
//const char* SYMBOL = "MIOTA";//iota
//const char* SYMBOL = "XTZ";//tezos
//const char* SYMBOL = "ATOM";//cosmos
//const char* SYMBOL = "ETC";//ethereum-classic
//const char* SYMBOL = "NEO";//neo
//const char* SYMBOL = "MKR";//maker
//const char* SYMBOL = "ONT";//ontology
//const char* SYMBOL = "XEM";//nem
//const char* SYMBOL = "BAT";//basic-attention-token
//const char* SYMBOL = "CRO";//crypto-com-chain
//const char* SYMBOL = "ZEC";//zcash
//const char* SYMBOL = "BTG";//bitcoin-gold
//const char* SYMBOL = "VET";//vechain
//const char* SYMBOL = "USDC";//usd-coin
//const char* SYMBOL = "DOGE";//dogecoin
//const char* SYMBOL = "DCR";//decred
//const char* SYMBOL = "LINK";//chainlink
//const char* SYMBOL = "TUSD";//trueusd
//const char* SYMBOL = "OMG";//omisego
//const char* SYMBOL = "QTUM";//qtum
//const char* SYMBOL = "WAVES";//waves
//const char* SYMBOL = "LSK";//lisk
//const char* SYMBOL = "REP";//augur
//const char* SYMBOL = "NANO";//nano
//const char* SYMBOL = "PAX";//paxos-standard-token
//const char* SYMBOL = "BCN";//bytecoin-bcn
//const char* SYMBOL = "RVN";//ravencoin
//const char* SYMBOL = "BCD";//bitcoin-diamond
//const char* SYMBOL = "ZRX";//0x
//const char* SYMBOL = "ICX";//icon
//const char* SYMBOL = "HOT";//holo
//const char* SYMBOL = "IOST";//iostoken
//const char* SYMBOL = "ABBC";//abbc-coin
//const char* SYMBOL = "ZIL";//zilliqa
//const char* SYMBOL = "BTS";//bitshares
//const char* SYMBOL = "BTT";//bittorrent
//const char* SYMBOL = "NPXS";//pundi-x
//const char* SYMBOL = "KMD";//komodo
//const char* SYMBOL = "AOA";//aurora
//const char* SYMBOL = "DGB";//digibyte
//const char* SYMBOL = "AE";//aeternity
//const char* SYMBOL = "HT";//huobi-token
//const char* SYMBOL = "XVG";//verge
//const char* SYMBOL = "SC";//siacoin
//const char* SYMBOL = "ENJ";//enjin-coin
//const char* SYMBOL = "STEEM";//steem
//const char* SYMBOL = "INB";//insight-chain
//const char* SYMBOL = "KCS";//kucoin-shares
//const char* SYMBOL = "BTM";//bytom
//const char* SYMBOL = "FCT";//factom
//const char* SYMBOL = "TRUE";//truechain
//const char* SYMBOL = "DAI";//dai
//const char* SYMBOL = "CNX";//cryptonex
//const char* SYMBOL = "QBIT";//qubitica
//const char* SYMBOL = "THR";//thorecoin
//const char* SYMBOL = "VEST";//vestchain
//const char* SYMBOL = "STRAT";//stratis
//const char* SYMBOL = "WTC";//waltonchain
//const char* SYMBOL = "SNT";//status
//const char* SYMBOL = "MCO";//crypto-com
//const char* SYMBOL = "XIN";//mixin
//const char* SYMBOL = "THETA";//theta
//const char* SYMBOL = "ZEN";//zencash
//const char* SYMBOL = "DGD";//digixdao
//const char* SYMBOL = "ARDR";//ardor
//const char* SYMBOL = "GNT";//golem-network-tokens
//const char* SYMBOL = "MAID";//maidsafecoin
//const char* SYMBOL = "MXM";//maximine-coin
//const char* SYMBOL = "WAX";//wax
//const char* SYMBOL = "PAI";//project-pai
//const char* SYMBOL = "JCT";//japan-content-token
//const char* SYMBOL = "DENT";//dent
//const char* SYMBOL = "MONA";//monacoin
//const char* SYMBOL = "ABT";//arcblock
//const char* SYMBOL = "NET";//next
//const char* SYMBOL = "ELF";//aelf
//const char* SYMBOL = "GXC";//gxchain
//const char* SYMBOL = "AION";//aion
//const char* SYMBOL = "R";//revain
//const char* SYMBOL = "ORBS";//orbs
//const char* SYMBOL = "PPT";//populous
//const char* SYMBOL = "ETP";//metaverse
//const char* SYMBOL = "SAN";//santiment
//const char* SYMBOL = "RLC";//rlc
//const char* SYMBOL = "MANA";//decentraland
//const char* SYMBOL = "ARK";//ark

const long DEFAULT_DELAY = 60 * 1000 * 5; // wait 5 minutes between API request;

const char* host = "pro-api.coinmarketcap.com";
const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
//const char fingerprint[] PROGMEM = "d3 79 2d dd ac 1f ed 90 8a a9 c1 c0 74 2d cf ff 6d 86 bf 0a";
//const char fingerprint[] PROGMEM = "A9 AE 42 11 94 5E 33 C6 27 83 22 71 01 A6 9D FC EE 0F AC 99";
//const char fingerprint[] PROGMEM = "ec 45 20 6c be b4 7d c1 bb 73 53 23 c2 03 77 f1 51 6a b2 61";
const char fingerprint[] PROGMEM = "335b4fc7b2e210256ed6f2f900744ea21f436195";

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


  //printPrice(99999999.0,);
  printPrice(999999, 1);
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
  DynamicJsonDocument jsonDocument(2024);
  DeserializationError error = deserializeJson(jsonDocument, response);
  if (error) {
    Serial.println("Deserialization failed");

    switch (error.code()) {
      case DeserializationError::Ok:
          Serial.print(F("Deserialization succeeded"));
          break;
      case DeserializationError::InvalidInput:
          Serial.print(F("Invalid input!"));
          break;
      case DeserializationError::NoMemory:
          Serial.print(F("Not enough memory"));
          Serial.println("doc capacity: ");
          Serial.print(jsonDocument.capacity());
          break;
      default:
          Serial.print(F("Deserialization failed"));
          break;
    }

    
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
  Serial.println(price);

  //find floating point index
  int left_part, right_part;
  char buffer[50];
  sprintf(buffer, "%lf", price);
  sscanf(buffer, "%d.%d", &left_part, &right_part);

  printPrice((int)left_part, (int)right_part);

  delay();
}

//void printPrice(int price){
void printPrice(int left_part, int right_part){
  // reset
  for(int index=0;index<lc.getDeviceCount();index++) {
    lc.clearDisplay(index);
  }

  int sofl = intLength(left_part);
  int sofr = intLength(right_part);
  int exss = min((8 - sofl), sofr);
  int index = floor((8-(sofl + exss)) /2);

  if(exss == 2){
    right_part = float(right_part / 10000);
  }else if(exss == 3){
    right_part = float(right_part / 1000);
  }else if(exss == 4){
    right_part = float(right_part / 100);
  }else if(exss == 5){
    right_part = float(right_part / 10);
  }

  while(exss > 0){
    lc.setDigit(0, index, right_part%10, false);
    right_part = right_part/10;
    index++;
    exss--;
  }

  bool pfFlag = true;
  while(sofl > 0){
    lc.setDigit(0, index, left_part%10, pfFlag);
    left_part = left_part/10;
    index++;
    sofl--;
    pfFlag = false;
  }
  
}

int intLength(int i){
  if(i > 9999999)
    return 8;
  else if(i > 999999)
    return 7;
  else if(i > 99999)
    return 6;
  else if(i > 9999)
    return 5;
  else if(i > 999)
    return 4;
  else if(i > 99)
    return 3;
  else if(i > 9)
    return 2;
  else
    return 1;
}
  
void delay() {
  Serial.println("");
  Serial.println("Keep calm and hodl!");
  delay(DEFAULT_DELAY);
}
