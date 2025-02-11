# ----- setup wor ESP32 NodeMCU -----
# ESP32 Dev Module 
#FQBN        ?= esp32:esp32:esp32
FQBN        ?= esp32:esp32:esp32c6

# ----- following setup for WiFi enabled devices
#IOT_NAME    ?= blink-arduino
#OTA_PORT   ?= 3232
#OTA_PASS   ?=
SERIAL_DEV  ?= /dev/ttyUSB0
