/*******************************************************************************
 ******************************************************************************/
//#include <../../../common/common.h>
//#include <SPI.h>
//#include <esp_now.h>
//#include <WiFi.h>

// status flags
//bool SETUP_OK = false;
//bool ESP_SETUP_OK = false;
//bool ESP_DATA_RECVD_OK = false;
//bool GPS_DATA_RECVD_OK = false;

// Create a struct_message to hold incoming data
//struct_message myData;

//callback function that will be executed when data is received
//void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
//  memcpy(&myData, incomingData, sizeof(myData));
//  Serial.print("Bytes received: ");
//  Serial.println(len);
//  Serial.println("GPS data: ");
//  Serial.println("  time: ");
//  Serial.print("    hour: "); Serial.println(myData.gps_time_hour);
//  Serial.print("    minute: "); Serial.println(myData.gps_time_minute);
  // Serial.println("-------");
//  ESP_DATA_RECVD_OK = true;
//}

void setup() {

  //Initialize Serial Monitor
  Serial.begin(115200);

  //Set device as a Wi-Fi Station
  //WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  // if (esp_now_init() != ESP_OK) {
  //   Serial.println("Error initializing ESP-NOW");
  //   return;
  // }
  //ESP_SETUP_OK = true;

  // Once ESPNow is successfully Init, we will register
  // for recv CB to get recv packer info
  //esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

}

void loop() {

  Serial.println("hello");
  //if (ESP_DATA_RECVD_OK) {
  //  if (myData.gps_status) {
  //    GPS_DATA_RECVD_OK = true;
  //  }
  //}
  delay(1000);
}
