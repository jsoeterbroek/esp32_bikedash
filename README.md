# esp32_bikedash

This hobby/learning project initially started out as a way to collect and display 
data from the Battery Monitor hardware module connected to my motorcycle battery 
via Bluetooth protocol.
Due to some severe scope creep it has now grown to include GPS and temperature
data. 

The setup consists of two parts: 1) the collector, responsible for collecting
data and sending to the 2) dashboard, which is responsible for displaying data.
The collector is going to be permanently attached to the bike and powered by the
12v battery and will be hidden out of sight under the sadle so it can also be
used as a future anti-theft device (with GSM integration). 
The display part will be powered via USB in a 3D-printed case and will be 
detachable to avoid unnecessary environmental exposure.

## Collector

### Power
Motorcycle battery

## Dashboard


### Power
USB

## Parts
see [PARTS.md](PARTS.md)

## TODO

### Dashboard
* Motorcycle gear display
* Motorcycle fuel display

### Anti-theft
* GSM integration

### ESP Home
* Integration with ESP Home (MQTT)

see also [TODO.txt](TODO.txt)

## Pictures

### Prototypes

* [Collector](dashboard/pics/IMG_0757.jpg)
* [Dashboard main screen](dashboard/pics/IMG_0760.jpg)
