# bm6-battery-monitor - Read data from BM6 BLE battery monitors
# https://github.com/jeffwdh/bm6-battery-monitor
#
# Wouldn't have been able to create this without the following resources:
# https://github.com/KrystianD/bm2-battery-monitor/blob/master/.docs/reverse_engineering.md
# https://doubleagent.net/bm2-reversing-the-ble-protocol-of-the-bm2-battery-monitor/
# https://www.youtube.com/watch?v=lhLff9VACU4

import argparse
import json
import asyncio
from Crypto.Cipher import AES
from bleak import BleakClient
from bleak import BleakScanner

# Function to scan for BM6 devices
async def scan_bm6(format):
  device_list = []
  scan = await BleakScanner.discover(return_adv=True, timeout=5)

  # Filter only BM6 devices
  for device in scan.values():
    if device[0].name == "Battery Monitor":
      device_list.append([device[0].address, device[1].rssi])

  # Output data
  if format == "ascii":
    if device_list:
      print("Address           RSSI")
      for item in device_list:
        print(item[0] + " " + str(item[1]))
    else:
      print("No BM6 devices found.")
  if format == "json":
    print(json.dumps(device_list))

AesBlockSize = 16
# Function to connect to a BM6 and pull voltage and temperature readings
# Note: Temperature readings are in Celsius and do not go below 0C
async def get_bm6_data(address, format):
  # The BM6 encryption key is only /slightly/ different than the BM2
  #key=bytearray([108, 101, 97, 103, 101, 110, 100, 255, 254, 48, 49, 48, 48, 48, 48, 57])
  key = b"\x6c\x65\x61\x67\x65\x6e\x64\xff\xfe\x31\x38\x38\x32\x34\x36\x36"
  print("foo")
  bm6_data = {
    "voltage": "",
    "temperature": "",
    "soc": ""
  }

  def decrypt(crypted):
    #cipher = AES.new(key, AES.MODE_CBC, 16 * b'\0')
    cipher = AES.new(key, AES.MODE_CBC, 16 * b'\0')
    decrypted = cipher.decrypt(crypted).hex()
    return decrypted

  def encrypt(plaintext):
    cipher = AES.new(key, AES.MODE_CBC, 16 * b'\0')
    encrypted = cipher.encrypt(plaintext)
    return encrypted

  async def notification_handler(sender, data):
    message = decrypt(data)
    if message[0:6] == "d15507": # Probably not needed, but voltage/temp messages start with d15507
      bm6_data["voltage"] = int(message[15:18],16) / 100
      bm6_data["soc"] = int(message[12:14],16)
      if message[6:8] == "01":
        bm6_data["temperature"] = -int(message[8:10],16)
      else:
        bm6_data["temperature"] = int(message[8:10],16)

  async with BleakClient(address, timeout=30) as client:
    # The d15507 command tells the BM6 to start sending volt/temp notifications
    await client.write_gatt_char("FFF3", encrypt(bytearray.fromhex("d1550700000000000000000000000000")), response=True)

    # Subscribe to notifications
    await client.start_notify("FFF4", notification_handler)

    # Wait for readings
    while not bm6_data["voltage"] and not bm6_data["temperature"]:
      await asyncio.sleep(0.1)

    # Clean up
    await client.stop_notify("FFF4")

    # Output data
    if format == "ascii":
      print("Voltage: " + str(bm6_data["voltage"]) + "v")
      print("Temperature: " + str(bm6_data["temperature"]) + "C")
      print("SoC: " + str(bm6_data["soc"]) + "%")
    if format == "json":
      print(json.dumps(bm6_data))

if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("--format", choices=["ascii", "json"], default="ascii", help="Output format")
  req = parser.add_mutually_exclusive_group(required=True)
  req.add_argument("--address", metavar="<address>", help="Address of BM6 to poll data from")
  req.add_argument("--scan", action="store_true", help="Scan for available BM6 devices")
  args = parser.parse_args()
  if args.address:
    try:
      asyncio.run(get_bm6_data(args.address, args.format))
    except Exception:
      pass
  if args.scan:
    try:
      asyncio.run(scan_bm6(args.format))
    except Exception:
      pass
