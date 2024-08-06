# HTTPS OTA example

Devices used for this example:
- ESP32-S3: Runs the OTA update program
- ESP32-C6: Used as an access point
- Computer: Acts as the firmware server


## Configuration

### ESP32

To get the IP address of the server:
- Open a CMD terminal
- Run the command: `ipconfig`
- Get the IPV4 address over WiFi

The server address will be https://server_ip:port/

To update the configuration of the ESP32:
- Run `idf.py menuconfig`
- Navigate to Example config -> Set WiFi SSID and WiFi Password
- Navigate to Example config -> Set firmware upgrade URL endpoint to https://server_ip:port/version.json


### Server

Create a certificate for the test server and copy it into the *server_certs/ca_cert.pem* and *server_certs/ca_key.pem* files.

Hierarchy of server files:

```sh
├── server
   ├── version1
   |     ├─ binaries 
   ├── version2
   |     ├─ binaries 
   ├── versionX
   |     ├─ binaries 
```

#### version.json
```
{
    "last_version" : 2.0,
    "bin_url" : "https://192.168.4.2:8080/v2.0/ESP32-OTA-WIFI.bin"
}
```

The version of the firmware is defined in the wifi_ota.c file, line 13
```
#define FIRMWARE_VERSION 2.0
```

The ESP32 will compare its firmware version (FIRMWARE_VERSION) to the last_version JSON property

The bin_url property indicates the location of the binaries for the defined version

Example: 
```
{
    "last_version" : 2.0,
    "bin_url" : "https://192.168.4.2:8080/v2.0/ESP32-OTA-WIFI.bin"
}
```
```sh
├── server
   ├── 1.0
   |    ├─  ESP32-OTA-WIFI.bin
   ├── 2.0
   |    ├─ ESP32-OTA-WIFI.bin    
```

Only devices with FIRMWARE_VERSION ≤ 1 will download this new version

#### Run the server

Run `python.exe .\python_server.py .\server\ 8080 .\server_certs\`

- .\server\: Folder that contains binaries and the version.json file
- 8080: Port
- .\server_certs\: Folder that contains server certificates

## Add new version

To add a new version, follow these steps:
- Update the firmware version to something greater than the current value, for example: #define FIRMWARE_VERSION 3.0
- Add a new line of code: ESP_LOGE(TAG, "Let's goooo --------------");
- Build the project to generate the new binaries.
- Add a new folder named v3.0 in the server directory and paste the latest built binaries into this folder.
- Update the version.json => "last_version": 3.0 "bin_url": "https://192.168.4.2:8080/v3.0/ESP32-OTA-WIFI.bin"


```
{
    "last_version" : 3.0,
    "bin_url" : "https://192.168.4.2:8080/v3.0/ESP32-OTA-WIFI.bin"
}
```
```sh
├── server
   ├── 1.0
   |    ├─  ESP32-OTA-WIFI.bin
   ├── 2.0
   |    ├─ ESP32-OTA-WIFI.bin
   ├── 3.0
   |    ├─ ESP32-OTA-WIFI.bin    
```

## Execution

```
I (3419) esp-x509-crt-bundle: Certificate validated
I (4309) ota_update: current firmware version (1.0) is lower than the available one (3.0), upgrading...
I (4309) ota_update: the new file name https://192.168.4.2:8080/v3.0/ESP32-OTA.bin
I (4319) ota_update: Attempting to download update from https://192.168.4.2:8080/v3.0/ESP32-OTA.bin
I (4549) esp-x509-crt-bundle: Certificate validated
I (4909) esp_https_ota: Starting OTA...
I (4909) esp_https_ota: Writing to partition subtype 16 at offset 0x110000
...
...
...
I (2544) esp_netif_handlers: netif_sta ip: 192.168.4.3, mask: 255.255.255.0, gw: 192.168.4.1
I (2544) WIFI: Got IPv4 event: Interface "netif_sta" address: 192.168.4.3
E (2544) ota_example: Let's goooo --------------
I (2554) ota_update: Starting OTA example task
I (2554) ota_update: Fetch json version file : https://192.168.4.2:8080/version.json
```




