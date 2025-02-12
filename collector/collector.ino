/*
*/
#include <SPI.h>
#include <Wire.h>
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
#define time_delay 2000

U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 7, 6, U8X8_PIN_NONE);
// end OLED display

bool SETUP_OK = false;
bool ESP_SETUP_OK = false;
bool ESP_SEND_OK = false;
bool GPS_OK = false;
bool GSM_OK = false;
bool TEMP_OK = false;
bool BATT_OK = false;

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
    ESP_SEND_OK = true;
  } else{
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

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } else {
    ESP_SETUP_OK = true;
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

  // output OLED
  u8g2.setCursor(0, 0);
  u8g2.print("// BMW R1100GS");
  u8g2.setCursor(0, 10);
  u8g2.print("   data collector");
  u8g2.setCursor(0, 20);
  u8g2.print("   setup..");
  u8g2.sendBuffer();
  delay(5000);
  SETUP_OK = true;
}
 
void loop() {
 
  // Set values to send
  // outgoingReadings.speed_kph = 13;
  // outgoingReadings.speed_rpm = 12;
  // outgoingReadings.fuel_perc = 50;
  
  // Send message via ESP-NOW
  // esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outgoingReadings, sizeof(outgoingReadings));
   
  // if (result == ESP_OK) {
  //   Serial.println("Sent with success");
  // } else {
  //   Serial.println("Error sending the data");
  // }

  // display status on LCD

  u8g2.clearBuffer();
  u8g2_prepare();

  u8g2.setFontMode(1);  /* activate transparent font mode */
  // TEMP_OK = true;

  // display setup status
  // display ESP setup status
  u8g2.setCursor(0, 0);
  u8g2.setDrawColor(2);
  if (ESP_SETUP_OK) {
    u8g2.print("ESP_SETUP_OK");
  } else {
    u8g2.setDrawColor(1);
    u8g2.drawBox(0, 0, 45, 10);
    u8g2.setDrawColor(2);
    u8g2.print("ESP_SETUP_NOK");
  }
  // display ESP send status
  u8g2.setCursor(0, 10);
  u8g2.setDrawColor(2);
  if (ESP_SEND_OK) {
    u8g2.print("ESP_SEND_OK");
  } else {
    u8g2.setDrawColor(1);
    u8g2.drawBox(0, 10, 80, 10);
    u8g2.setDrawColor(2);    
    u8g2.print("ESP_SEND_NOK");
  }
  // display GPS status
  u8g2.setCursor(0, 20);
  u8g2.setDrawColor(2);
  if (GPS_OK) {
    u8g2.print("GPS_OK");
  } else {
    u8g2.setDrawColor(1);
    u8g2.drawBox(0, 20, 80, 10);
    u8g2.setDrawColor(2);
    u8g2.print("GPS_NOK");
  }
  // display GSM status
  u8g2.setCursor(0, 30);
  u8g2.setDrawColor(2);
  if (GSM_OK) {
    u8g2.print("GSM_OK");
  } else {
    u8g2.setDrawColor(1);
    u8g2.drawBox(0, 30, 80, 10);
    u8g2.setDrawColor(2);
    u8g2.print("GSM_NOK");
  }
  // display TEMPstatus
  u8g2.setCursor(0, 40);
  u8g2.setDrawColor(2);
  if (TEMP_OK) {
    u8g2.print("TEMP_OK");
  } else {
    u8g2.setDrawColor(1);
    u8g2.drawBox(0, 40, 80, 10);
    u8g2.setDrawColor(2);    
    u8g2.print("TEMP_NOK");
  }
  // display BATT status
  u8g2.setCursor(0, 50);
  u8g2.setDrawColor(2);
  if (BATT_OK) {
    u8g2.print("BATT_OK");
  } else {
    u8g2.setDrawColor(1);
    u8g2.drawBox(0, 50, 80, 10);
    u8g2.setDrawColor(2);    
    u8g2.print("BATT_NOK");
  }

  u8g2.sendBuffer();
  delay(2000);

}
