# ----- setup wor ESP32 NodeMCU -----
#esp32-c6-devkitc-1
FQBN        ?= esp32:esp32:esp32c6

# ----- following setup for WiFi enabled devices
IOT_IP     ?= 192.168.178.38
IOT_NAME   ?= collector
OTA_PORT   ?= 3232
OTA_PASS   ?= admin
SERIAL_DEV ?= /dev/ttyUSB0
