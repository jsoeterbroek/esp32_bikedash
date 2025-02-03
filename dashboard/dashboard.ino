/*
 Collector
  A = collector MAC: FC:B4:67:F6:90:E0
  B = dashboard MAC: D8:BC:38:FA:D4:A4
*/
#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0xfc, 0xb4, 0x67, 0xf6, 0x90, 0xe0};


//Structure example to received data
//Must match the sender's structure
typedef struct struct_speedo {
    uint8_t speed_kph;
    uint8_t speed_rpm;
    int8_t fuel_perc;
} struct_speedo;

// Create a struct_message to hold incoming readings
struct_speedo SpeedoData;

esp_now_peer_info_t peerInfo;

// callback function that will be executed when data is received
void OnDataRecv(const esp_now_recv_info* mac, const uint8_t *incomingData, int len) {
  memcpy(&SpeedoData, incomingData, sizeof(SpeedoData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Speed KPH: ");
  Serial.println(SpeedoData.speed_kph);
  Serial.print("Speed RPM: ");
  Serial.println(SpeedoData.speed_rpm);
  Serial.print("Fuel Percentage: ");
  Serial.println(SpeedoData.fuel_perc);
  Serial.println();
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {

}
