# ----- setup wor ESP32 NodeMCU -----
#esp32-c6-devkitc-1
FQBN        = esp32:esp32:esp32c6

SERIAL_DEV = /dev/ttyUSB0
SPEEDLABEL = 921600
# ----- following setup for WiFi enabled devices
IOT_IP     = 192.168.178.38
IOT_NAME   = collector.local
OTA_PORT   = 3232
OTA_PASS   = admin
