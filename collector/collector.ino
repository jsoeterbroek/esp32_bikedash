/*
*/
#include <SPI.h>
#include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <SH1106.h>
#include "U8g2lib.h"
#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH THE MAC Address of your receiver (dashboard)
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


// PINOUTS
// 
// LCD oled display
//  I2C
// VCC  ---> 3V3, 
// GND  ---> GND, 
// SDA  ---> LP_I2C_SDA  ->  pin 6
// SCL  ---> LP_I2C_SCL  ->  pin 7

// start OLED display
#define OLED_SDA 6
#define OLED_SCL 7

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
//U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 7, /* dc=*/ 6);
// end OLED display

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    uint8_t speed_kph;
    uint8_t speed_rpm;
    int8_t fuel_perc;
} struct_message;

// Create a struct_message to hold outgoing readings
struct_message outgoingReadings;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

void u8g2_prepare() {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // initialize OLED
  u8g2.begin();
  u8g2_prepare();
  delay(2000);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
 
  // Set values to send
  outgoingReadings.speed_kph = 13;
  outgoingReadings.speed_rpm = 12;
  outgoingReadings.fuel_perc = 50;
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outgoingReadings, sizeof(outgoingReadings));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }

  // display status on LCD
    u8g2.clearBuffer();
    u8g2_prepare();
    u8g2.setCursor(0, 0);
    u8g2.print("ESP_OK");

}
