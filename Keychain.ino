#include <Adafruit_MPU6050.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <Preferences.h>
#include <WebServer.h>
#include <DHT11.h>
#include <time.h>
#include "config.h"

//Initialize Display Pins
#define SC_WD 128
#define SC_HT 64
#define OLED_RESET -1

//Initialize SPI Pins
/*Notes for another SPI PINS : 
SCK = 23, MOSI = 22, MISO = 21, CLK = 19
*/
#define PIN_CS 23

//Preferences Features for WiFi
Preferences prefs;
WebServer server(80);
String iSSID = "", iPass = "";
const char* AP_SSID = ConfigAP_SSID;
const char* AP_PASS = ConfigAP_PASS;

//Wifi ID
const char* DefaultSSID[] = ConfigSSID[SELECTED_SSID];
const char* DefaultPass[] = ConfigPass[SELECTED_SSID];
uint8_t connected = 0;
uint8_t timeout = 0;
const unsigned long WiFiTimeout = 10000;

//NTP Server
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0;

//DEKLARASI FUNGSI
void connect(uint8_t *connected);
void displayText(char *text[]);
struct tm NTPtime(uint8_t connected);
void displaytext(char *text[], uint8_t max);
void operateMPU();

Adafruit_SSD1306 display(SC_WD, SC_HT, &Wire, OLED_RESET);
Adafruit_MPU6050 mpu;

struct logs{
    struct tm log_time;

}

void setup(){
    Serial.begin(115200);

    //Display initialization
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_RESET)){
        Serial.println(F("Failed allocating Display"));
        for(;;);
    }

    //SD Card initialization
    if(!SD.begin(PIN_CS)){
        Serial.println("Failed initializing SD Card");
        return;
    }

    //MPU6050 initialization
    if(!mpu.begin()){
      Serial.println("Failed to initialize MPU6050 chip");
      while(1){
        delay(10);
      }
    }
}

//DEKARASI FUNGSI DETAIL
void connect(uint8_t *connected){
    WiFi.mode(WIFI_STA);

    //Start the default Wi-Fi
    for(int i = 0; i < 2; i++){
        WiFi.begin(DefaultSSID[i], DefaultPass[i]);
        unsigned long t0 = millis();

        while(millis() - t0 < 10000){
            if(WiFi.status() == WL_CONNECTED){
                *connected = 1;
                return;
            }
        }
    }

    //Start the alternative WiFi
    prefs.begin("wifi", false);
    iSSID = prefs.getString("ssid", "");
    iPass = prefs.getString("pass", "");
    prefs.end();

    if(iSSID != ""){
        WiFi.begin(iSSID.c_str(), iPass.c_str());
    
        Serial.print("Connecting to saved WiFi : ");
        Serial.println(iSSID);
        unsigned long WiFiStart = millis();

        while(millis() - WiFiStart < WiFiTimeout){
            if(WiFi.status() == WL_CONNECTED){
                Serial.println("WiFi Connected!");
                Serial.print("IP : ");
                Serial.println(WiFi.localIP());
                *connected = 1;
                return;
            }
            delay(200);
        }
        Serial.println("Wifi Connection Timeout!");
        *connected = 0;
    }

    // Kalau gagal, buka AP
    Serial.println("\nStarting AP mode...");
    WiFi.softAP("ESP32_Setup", "12345678");
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    // Halaman web
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", R"rawliteral(
        <form action="/save" method="get">
            SSID: <input name="ssid"><br>
            Password: <input name="pass"><br>
            <input type="submit" value="Save">
        </form>
        )rawliteral");
    });

    server.on("/save", HTTP_GET, []() {
        if (server.hasArg("ssid") && server.hasArg("pass")) {
        iSSID = server.arg("ssid");
        iPass = server.arg("pass");

        prefs.begin("wifi", false);
        prefs.putString("ssid", iSSID);
        prefs.putString("pass", iPass);
        prefs.end();
            
        String resp = "Saved. Rebooting...";
        server.send(200, "text/html", resp);
        delay(3000);
        ESP.restart();
        } else {
        server.send(400, "text/plain", "Missing SSID or Password");
        }
    });

    NTPtime(connected);

    server.begin();
    WiFi.disconnect(true);
}

struct tm NTPtime(uint8_t *connected){
    struct tm timeinfo;
    if(connected){
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        if(getLocalTime(&timeinfo)){
            Serial.println("Failed while fetching NTP time");
            memset(&timeinfo, 0, sizeof(timeinfo));
        }    
    }
    
    else{
        memset(&timeinfo, 0, sizeof(timeinfo));
    }
    return timeinfo;
}

//Declare it by using displaytext(text, sizeof(text)/text([0]))
void displaytext(char *text[], uint8_t max){
    display.clearDisplay();                    
    display.setTextSize(1);                    
    display.setTextColor(SSD1306_WHITE);       
    
    for(uint8_t i = 0; i < max; i++){
        display.setCursor(0, i*8);                   
        display.println(text[i]);
    }
    display.display();        
}

void startMPU(){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  Serial.print("Accelerations in m/s^2 (x, y, z) : ");
  Serial.print(a.acceleration.x, a.acceleration.y, a.acceleration.z);

  Serial.print("Rotations in rad/s (x, y, z) : ");
  Serial.print(g.gyro.x, g.gyro.y, g.gyro.z);

  Serial.print("Temperature in C : ");
  Serial.print(temp.temperature);
}

void write_events;