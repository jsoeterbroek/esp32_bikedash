/*
*/
#include <SPI.h>
#include <esp_now.h>
#include <WiFi.h>
#include <TFT_eSPI.h>

// PINOUTS
// 
// VCC           3V
// GND           GND
// DIN           GPIO 23
// CLK           GPIO 18
// CS            GPIO 15
// DC            GPIO 2
// RST           GPIO 4
// BL            not connected

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    int id;
    float temp;
    float hum;
    unsigned int readingId;
    uint8_t speed_kph;
    uint8_t speed_rpm;
    int8_t fuel_perc;
} struct_message;


// Create a struct_message to hold incoming data
struct_message myData;

//callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("temp: ");
  Serial.println(myData.temp);  
  Serial.print("hum: ");
  Serial.println(myData.hum);
}

void setup() {
  //Initialize Serial Monitor
  Serial.begin(115200);
  
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

    // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}
 
void loop() {

}
