# Zigbee OTA example

Devices used for this example:
- ESP32-H2: Runs the server program
- ESP32-C6: Runs the client program

## Configuration

#### ESP32-H2
- Run idf.py menuconfig
- Navigate to Component config -> Zigbee -> Set Zigbee enable to true
- Navigate to Partition table -> Partition Table -> Set Custom partition table CSV to true

#### ESP32-C6
- Run idf.py menuconfig
- Navigate to Component config -> Zigbee -> Set Zigbee enable to true
- Navigate to Partition table -> Partition Table -> Set Custom partition table CSV to true
- Navigate to Serial flasher config -> Set Flash size to 4MB

## Run
- Build and flash the client.
- Add a new line of code in the esp_ota_client.c.
- Build the client again.
- Copy and paste the client build binaries into the server's main folder and rename it to ota_file_zigbee.bin.
- Build and flash the server.

The server will be available for Zigbee connection. Restart the client if needed to re-establish the connection to the Zigbee server. The client will send a request to the server to get the new binaries. The server will send packets of 64 bytes each to the client. When the process is finished, the client will restart to execute the downloaded binaries.


ESP32-H2 (Server)
```
I (1256) ESP_OTA_SERVER: Network(0x34bd) is open for 180 seconds
I (1616) ESP_OTA_SERVER: ZDO signal: ZDO Device Update (0x30), status: ESP_OK
I (1626) ESP_OTA_SERVER: New device commissioned or rejoined (short: 0x789f)
I (1626) ESP_OTA_SERVER: Notify OTA upgrade
I (1776) ESP_OTA_SERVER: OTA upgrade server query image
I (1776) ESP_OTA_SERVER: OTA client address: 0x789f
I (1776) ESP_OTA_SERVER: OTA version: 0x1010101, image type: 0x1011, manufacturer code: 1001,
I (1786) ESP_OTA_SERVER: OTA table index: 0x0
I (2576) ESP_OTA_SERVER: -- OTA Server transmits data from 0x0 to 0x789f: progress [8/616160]
I (2576) ESP_OTA_SERVER: OTA client address: 0x789f
I (2576) ESP_OTA_SERVER: OTA version: 0x1010110, image type: 0x1011, server status: 0
I (2806) ESP_OTA_SERVER: -- OTA Server transmits data from 0x0 to 0x789f: progress [72/616160]
I (3026) ESP_OTA_SERVER: -- OTA Server transmits data from 0x0 to 0x789f: progress [136/616160]
I (3246) ESP_OTA_SERVER: -- OTA Server transmits data from 0x0 to 0x789f: progress [200/616160]
I (3466) ESP_OTA_SERVER: -- OTA Server transmits data from 0x0 to 0x789f: progress [264/616160]
I (3686) ESP_OTA_SERVER: -- OTA Server transmits data from 0x0 to 0x789f: progress [328/616160]
.....
.....
.....
.....
I (2154276) ESP_OTA_SERVER: -- OTA Server transmits data from 0x0 to 0x789f: progress [616008/616160]
I (2154496) ESP_OTA_SERVER: -- OTA Server transmits data from 0x0 to 0x789f: progress [616072/616160]
I (2154716) ESP_OTA_SERVER: -- OTA Server transmits data from 0x0 to 0x789f: progress [616136/616160]
I (2154936) ESP_OTA_SERVER: -- OTA Server transmits data from 0x0 to 0x789f: progress [0/616160]
I (2154976) ESP_OTA_SERVER: OTA client address: 0x789f
I (2154976) ESP_OTA_SERVER: OTA version: 0x1010110, image type: 0x1011, server status: 2
I (2154976) ESP_OTA_SERVER: OTA upgrade time: 0x1234a
I (2163246) ESP_OTA_SERVER: ZDO signal: ZDO Device Update (0x30), status: ESP_OK
```


ESP32-C6 (Client)
```
I (501) ESP_OTA_CLIENT: Initialize Zigbee stack
I (2871) ESP_OTA_CLIENT: Device started up in non factory-reset mode
I (2871) ESP_OTA_CLIENT: Device rebooted
I (3061) ESP_OTA_CLIENT: Queried OTA image from address: 0x0000, endpoint: 5
I (3061) ESP_OTA_CLIENT: Image version: 0x1010110, manufacturer code: 0x1001, image size: 616216
I (3061) ESP_OTA_CLIENT: Approving OTA image upgrade
I (3071) ESP_OTA_CLIENT: -- OTA upgrade start
I (3861) ESP_OTA_CLIENT: -- OTA Client receives data: progress [8/616160]
I (4071) ESP_OTA_CLIENT: -- OTA Client receives data: progress [72/616160]
I (4291) ESP_OTA_CLIENT: -- OTA Client receives data: progress [136/616160]
I (4511) ESP_OTA_CLIENT: -- OTA Client receives data: progress [200/616160]
I (4731) ESP_OTA_CLIENT: -- OTA Client receives data: progress [264/616160]
I (4951) ESP_OTA_CLIENT: -- OTA Client receives data: progress [328/616160]
.....
.....
.....
.....
I (2155751) ESP_OTA_CLIENT: -- OTA Client receives data: progress [616072/616160]
I (2155971) ESP_OTA_CLIENT: -- OTA Client receives data: progress [616136/616160]
I (2156191) ESP_OTA_CLIENT: -- OTA Client receives data: progress [616160/616160]
I (2156191) ESP_OTA_CLIENT: -- OTA upgrade check status: ESP_OK
I (2156231) ESP_OTA_CLIENT: -- OTA upgrade apply
I (2161241) ESP_OTA_CLIENT: -- OTA Finish
I (2161241) ESP_OTA_CLIENT: -- OTA Information: version: 0x1010110, manufacturer code: 0x1001, image type: 0x1011, total size: 616160 bytes, cost time: 2158162 ms,
I (2161241) esp_image: segment 0: paddr=00110020 vaddr=42080020 size=11064h ( 69732) map
I (2161261) esp_image: segment 1: paddr=0012108c vaddr=40800000 size=0e794h ( 59284)
I (2161271) esp_image: segment 2: paddr=0012f828 vaddr=4080e7a0 size=007f0h (  2032) 
I (2161271) esp_image: segment 3: paddr=00130020 vaddr=42000020 size=74e24h (478756) map
I (2161341) esp_image: segment 4: paddr=001a4e4c vaddr=4080ef90 size=01868h (  6248) 
I (2161341) esp_image: segment 0: paddr=00110020 vaddr=42080020 size=11064h ( 69732) map
I (2161361) esp_image: segment 1: paddr=0012108c vaddr=40800000 size=0e794h ( 59284) 
I (2161371) esp_image: segment 2: paddr=0012f828 vaddr=4080e7a0 size=007f0h (  2032)
I (2161371) esp_image: segment 3: paddr=00130020 vaddr=42000020 size=74e24h (478756) map
I (2161441) esp_image: segment 4: paddr=001a4e4c vaddr=4080ef90 size=01868h (  6248) 
W (2161481) ESP_OTA_CLIENT: Prepare to restart system
.....
.....
.....
.....
W (547) ESP_OTA_CLIENT: ESP Zigbee OTA example 1.0 is running
E (567) ESP_OTA_CLIENT: Let's gooooooooooooooooooooooo----------------------!!!!!!!!!!!!!!!!!!!!!
I (577) phy_init: phy_version 290,81efd96,May  8 2024,10:42:13
I (607) phy: libbtbb version: f97b181, May  8 2024, 10:42:29
I (627) ESP_OTA_CLIENT: ZDO signal: ZDO Config Ready (0x17), status: ESP_FAIL
I (627) ESP_OTA_CLIENT: Initialize Zigbee stack
```

