# ----- setup for ESP32 NodeMCU -----
#
# ESP32 Dev Module
#FQBN        ?= esp32:esp32:esp32
FQBN        ?= esp32:esp32:esp32c6

# ESP32 with TFT screen Esp32-2432S032C 
# - 3.2 inch TFT-LCD Display 320*240
# FQBN        ?= esp32:esp32:jczn_2432s028r ??

# ----- following setup for WiFi enabled devices
#IOT_NAME    ?= blink-arduino
#OTA_PORT   ?= 3232
#OTA_PASS   ?=
SERIAL_DEV  ?= /dev/ttyUSB0
SERIAL_BAUD ?= 115200
