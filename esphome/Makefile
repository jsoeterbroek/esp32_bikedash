# Makefile for esphome
#
# source venv/bin/activate
#
-include vars.mak

# ----- setup wor ESP32 NodeMCU -----
FQBN        ?= esp32:esp32:esp32-s3


all: compile run
.PHONY: run

compile:
	@esphome compile esp32-bikedash-esphome-client.yaml

run:
	@esphome run esp32-bikedash-esphome-client.yaml 
