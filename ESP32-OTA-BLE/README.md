# ESP BLE OTA

This example is based on the espressif ``ble ota component``, it receives firmware via Bleutooth Low Energy (BLE) and writes it to flash, sector by sector, until the upgrade is complete.

Here are some terms used in Bluetooth:
- Server: End device, like the ESP32 in this case
- Client: Usually refers to the server that will send the firmware to the device

<br/>

# SERVER PART (ESP32)

## 1. Services definition

The component contains two services:

- `DIS Service`: Displays software and hardware version information
- `OTA Service`: It is used for OTA upgrade and contains 4 characteristics, as shown in the following table:

|  Characteristics   | UUID  |  Prop   | description  |
|  ----  | ----  |  ----  | ----  |
|  RECV_FW_CHAR | 0x8020 | Write, notify  | Firmware received, send ACK |
|  PROGRESS_BAR_CHAR  | 0x8021 | Read, notify  | Read the progress bar and report the progress bar |
|  COMMAND_CHAR  | 0x8022 | Write, notify  | Send the command and ACK |
|  CUSTOMER_CHAR  | 0x8023 | Write, notify  | User-defined data to send and receive |

## 2. Data transmission

### 2.1 Command package format

|  unit   | Command_ID  |  PayLoad   | CRC16  |
|  ----  | ----  |  ----  | ----  |
|  Byte | Byte: 0 ~ 1 | Byte: 2 ~ 17  | Byte: 18 ~ 19 |

Command_ID:

- 0x0001: Start OTA, Payload bytes(2 to 5), indicates the length of the firmware. Other Payload is set to 0 by default. CRC16 calculates bytes(0 to 17).
- 0x0002: Stop OTA, and the remaining Payload will be set to 0. CRC16 calculates bytes(0 to 17).
- 0x0003: The Payload bytes(2 or 3) is the payload of the Command_ID for which the response will be sent. Payload bytes(4 to 5) is a response to the command. 0x0000 indicates accept, 0x0001 indicates reject. Other payloads are set to 0. CRC16 computes bytes(0 to 17).

### 2.2 Firmware package format

The format of the firmware package sent by the client is as follows:

|  unit   | Sector_Index  |  Packet_Seq   | PayLoad  |
|  ----  | ----  |  ----  | ----  |
|  Byte | Byte: 0 ~ 1 | Byte: 2  | Byte: 3 ~ (MTU_size - 4) |

- Sector_Index：Indicates the number of sectors, sector number increases from 0, cannot jump, must be send 4K data and then start transmit the next sector, otherwise it will immediately send the error ACK for request retransmission.
- Packet_Seq：If Packet_Seq is 0xFF, it indicates that this is the last packet of the sector, and the last 2 bytes of Payload is the CRC16 value of 4K data for the entire sector, the remaining bytes will set to 0x0. Server will check the total length and CRC of the data from the client, reply the correct ACK, and then start receive the next sector of firmware data.

The format of the reply packet is as follows:

|  unit   | Sector_Index  |  ACK_Status   | CRC6  |
|  ----  | ----  |  ----  | ----  |
|  Byte | Byte: 0 ~ 1 | Byte: 2 ~ 3  | Byte: 18 ~ 19 |

ACK_Status:

- 0x0000: Success
- 0x0001: CRC error
- 0x0002: Sector_Index error, bytes(4 ~ 5) indicates the desired Sector_Index
- 0x0003：Payload length error

## 3. NimBLE OTA

#### Configure the Project

`idf.py menuconfig`

- Component config → Bluetooth → Bluetooth → Host → NimBLE - BLE only
- Partition Table -> Partition Table -> Custom partition table csv

Note: For maximum throughput, set maximum MTU using

- Component config → Bluetooth → NimBLE Options → Preferred MTU size in octets as 517

## 4. Build and Run

- Add an RSA key or use the default one in this example: rsa_key/private.pem.
- Build and flash.

<br/>

# CLIENT PART (computer)

Run the Python program that will send the last build to the device over Bluetooth. The program will scan BLE devices, prompt for the device to connect to, and request the path of the file to be sent to the device.

Now, update the app_main.c file and add the following line of code at line 364.
```
ESP_LOGE(TAG, "Let's gooooo -----------------------------");
```


So now, we can run the bluetooth client and use the last built binaries

- python.exe .\python\main.py

```
Scanning for devices...
39:76:19:6C:EB:BD: None
C8:F0:9E:2B:E0:CE: nimble-ble-ota
7C:0C:1A:DB:38:DE: None
Enter the address of the ESP32 device: C8:F0:9E:2B:E0:CE
Enter the path to the firmware file to send: .........\ESP32-OTA-BLE\build\ble_ota.bin
```

# EXECUTION

Client Part (python)
```
Connected to C8:F0:9E:2B:E0:CE
Sent command 1 with payload 40840800 and CRC 791d
Sent packet for sector 0, packet_seq 0
Sent packet for sector 0, packet_seq 1
Sent packet for sector 0, packet_seq 2
Sent packet for sector 0, packet_seq 3
Sent packet for sector 0, packet_seq 4
Sent packet for sector 0, packet_seq 5
Sent packet for sector 0, packet_seq 6
Sent packet for sector 0, packet_seq 7
Sent packet for sector 0, packet_seq 255
Sent packet for sector 1, packet_seq 0
Sent packet for sector 1, packet_seq 1
Sent packet for sector 1, packet_seq 2
Sent packet for sector 1, packet_seq 3
Sent packet for sector 1, packet_seq 4
Sent packet for sector 1, packet_seq 5
Sent packet for sector 1, packet_seq 6
Sent packet for sector 1, packet_seq 7
Sent packet for sector 1, packet_seq 255
...
...
Sent packet for sector 135, packet_seq 255
Sent packet for sector 136, packet_seq 0
Sent packet for sector 136, packet_seq 1
Sent packet for sector 136, packet_seq 255
Sent command 2 with payload 00000000000000000000000000000000 and CRC e8b5
OTA update complete
```

Server part (ESP32)
```
I (309168) ESP_BLE_OTA: recv: 4096, recv_total:20480

I (309168) NimBLE_BLE_OTA: Notify is disabled
I (310368) ESP_BLE_OTA: recv: 4096, recv_total:24576

I (310368) NimBLE_BLE_OTA: Notify is disabled
I (311568) ESP_BLE_OTA: recv: 4096, recv_total:28672

I (311568) NimBLE_BLE_OTA: Notify is disabled
I (312708) ESP_BLE_OTA: recv: 4096, recv_total:32768

I (312708) NimBLE_BLE_OTA: Notify is disabled
I (313908) ESP_BLE_OTA: recv: 4096, recv_total:36864

...
...

I (453) coexist: coex firmware version: d96c1e51f
I (458) main_task: Started on CPU0
I (468) main_task: Calling app_main()
I (478) BTDM_INIT: BT controller compile version [4012cfb]
I (478) BTDM_INIT: Bluetooth MAC: c8:f0:9e:2b:e0:ce
I (488) phy_init: phy_version 4791,2c4672b,Dec 20 2023,16:06:06
I (798) NimBLE_BLE_OTA: BLE Host Task Started
E (798) ESP_BLE_OTA: Let's gooooo -----------------------------
I (808) ESP_BLE_OTA: ota_task start
```


