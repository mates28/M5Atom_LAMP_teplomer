#include <M5Atom.h>
/*#include <Wire.h>*/
#include <WiFi.h>
#include <HTTPClient.h>
/*#include "Adafruit_Sensor.h"
#include <Adafruit_BMP280.h>
#include "SHT3X.h"*/

String WIFI_ssid     = "SPSELIT-IoT";
String WIFI_password = "SPSIoT2023";

const char* serverName = "http://172.16.17.113/php/sensor01-data-post.php";
String apiKeyValue = "c6e14580-2ee8-4dd1-bb60-a155fb028c3a";
String sensorName = "M5 ATOM Sensor01";
String sensorLocation = "Učebna LABv";

extern const unsigned char Disconnect_ico[77];
extern const unsigned char Connect_ico[77];
extern const unsigned char Timeout_ico[77];
 
int update_int = 60000;
int tims = update_int;

HTTPClient http;
/*SHT3X sht30;
Adafruit_BMP280 bme;*/

float tmp = 0.0;
float outdoorTmp = 0.0;
float hum = 0.0;
float pressure = 0.0;
float seaPressure = 0.0;
float alt = 0;

// Led symbols definition ------------------------------------------------------ //
void Disconnect_led() {
  M5.dis.clear();
  delay(100);
  M5.dis.displaybuff((uint8_t*)Disconnect_ico);
  FastLED.show();
}
void Connect_led() {
  M5.dis.clear();
  delay(100);
  M5.dis.displaybuff((uint8_t*)Connect_ico);
  FastLED.show();
}
void Timeout_led() {
  M5.dis.clear();
  delay(250);
  M5.dis.displaybuff((uint8_t*)Timeout_ico);
  FastLED.show();
}

// ----------------------------------------------------------------------------- //

boolean connect_Wifi(const char * ssid, const char * password){
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname("ATOM_sensor01");
  WiFi.begin(ssid, password);
  delay(1000);
  wl_status_t wifi_Status = WiFi.status();  
  char SSIDbuff[64];
  while (wifi_Status != WL_CONNECTED) {
    wifi_Status = WiFi.status();
    switch(wifi_Status){
      case WL_NO_SSID_AVAIL:
          sprintf(SSIDbuff,"Nevidím %s",ssid);
          Serial.println(SSIDbuff);
          break;
      case WL_CONNECT_FAILED:
          Serial.println("Connection failed");
          break;
      case WL_CONNECTION_LOST:
          Serial.println("Connection lost");
          break;
      case WL_DISCONNECTED:
          Serial.println("WiFi odpojeno");
          break;
      case WL_IDLE_STATUS:
          Serial.println("[CHYBA] WiFi idle status");
          break;
      case WL_SCAN_COMPLETED:
          Serial.println("[OK] WiFi scan completed");
          break;
      case WL_CONNECTED:
          sprintf(SSIDbuff,"Pripojeno %s",ssid);
          Serial.println(SSIDbuff);
          break;
      default:
          Serial.println("Neznama chyba");
          break;
    }
    delay(500);
  }
  if (wifi_Status == WL_CONNECTED) {
    Serial.print("IP adresa: ");
    Serial.println(WiFi.localIP());
    Connect_led();
    return true;
  } else {
    Serial.println("Nepřipojeno " + String(ssid));
    Timeout_led();
    delay(1000);
    return false;
  }
}

void sendHttpData() {
  http.begin(serverName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  String httpRequestData = "api_key=" + apiKeyValue + "&typ_sensoru=" + sensorName
                            + "&umisteni=" + sensorLocation + "&teplota=" + String(tmp)
                            + "&vlhkost=" + String(hum) + "&atm_tlak=" + String(pressure)
                            + "&n_vyska=" + String(alt) + "";
  
  int httpResponseCode = http.POST(httpRequestData);
  if (httpResponseCode>0) {
     Serial.print("HTTP Response code: ");
     Serial.println(httpResponseCode);
  } else {
     Serial.print("HTTP Error code: ");
     Serial.println(httpResponseCode);
  }
  http.end();
}

void printData() {
  Serial.printf("Teplota: %2.1f °C", tmp);
  Serial.println("");
  Serial.printf("Vlhkost: %2.0f %%", hum);
  Serial.println("");
  Serial.printf("Atmosferický tlak: %2.0f hPa", pressure);
  Serial.println("");
  Serial.printf("Nadmořská výška: %2.0f m", alt);
  Serial.println("");
  sendHttpData();
}

void setup() {
  M5.begin(true,true,true);
  M5.dis.setBrightness(20);
  delay(200);
  Disconnect_led();
  //Wire.begin(26, 32);
  //
  if (WiFi.status() != WL_CONNECTED) {
    int WIFI_location = 0;
    while(WiFi.status() != WL_CONNECTED) {
      delay(500);
      connect_Wifi(WIFI_ssid.c_str(), WIFI_password.c_str());
    }
  }
  delay(500);
  //
  //M5.dis.clear();
  delay(200);
  //
  /*while (!bme.begin(0x76)){  
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    //M5.Lcd.println("Could not find a valid BMP280 sensor, check wiring!");
  }*/
}
 
void loop() {
  M5.update();
  
  if (tims >= update_int) {
    /*if (sht30.get()==0) {
      tmp = sht30.cTemp;
      hum = sht30.humidity;
    }
    pressure = bme.readPressure()/100;
    outdoorTmp = bme.readTemperature();
    Serial.print("BMP280 Temp: ");
    Serial.println(outdoorTmp);
    seaPressure = pressure/pow(1.0-287/44330.0,5.255); // indoor BOSCH definition
    //seaPressure = pressure/pow(1.0-0.0065*287/(outdoorTmp+273.15),5.255); // outdoor BOSCH definition
    alt = bme.readAltitude(seaPressure);*/
    
    // fiktivní random hodnoty
    tmp = (String(20+random(10))+"."+String(random(10))).toFloat();
    hum = 40+random(30);
    pressure = 850+random(200);
    alt = 287;
    //
    
    printData();
    tims = 0;
  }
  
  tims++;
  delay(1);
}
