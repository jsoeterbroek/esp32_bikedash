/*
*/
#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH THE MAC Address of the receiver (dashboard)
// 1c:69:20:cd:4c:e8
uint8_t broadcastAddress[] = {0x1c, 0x69, 0x20, 0xcd, 0x4c, 0xe8};

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    uint8_t speed_kph;
    uint8_t speed_rpm;
    int8_t fuel_perc;
    bool collector_is_dev;
} struct_message;

// Create a struct_message to hold outgoing readings
struct_message outgoingData;

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

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

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
 
void send_data() {
 
  // Set initial values to send
  outgoingData.speed_kph = 13;
  outgoingData.speed_rpm = 12;
  outgoingData.fuel_perc = 50;
  outgoingData.collector_is_dev = true;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outgoingData, sizeof(outgoingData));
  if (result != ESP_OK) {
    Serial.println("Error sending the data");
  }
}

void loop() {
  send_data();
}
