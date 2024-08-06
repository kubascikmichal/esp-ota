import asyncio
import struct
import crcmod
from bleak import BleakClient, BleakScanner

# Define UUIDs
DIS_SERVICE_UUID = "0000180a-0000-1000-8000-00805f9b34fb"
OTA_SERVICE_UUID = "00008020-0000-1000-8000-00805f9b34fb"
RECV_FW_CHAR_UUID = "00008020-0000-1000-8000-00805f9b34fb"
PROGRESS_BAR_CHAR_UUID = "00008021-0000-1000-8000-00805f9b34fb"
COMMAND_CHAR_UUID = "00008022-0000-1000-8000-00805f9b34fb"
CUSTOMER_CHAR_UUID = "00008023-0000-1000-8000-00805f9b34fb"

START_OTA_CMD = 0x0001
STOP_OTA_CMD = 0x0002

# CRC16 function
crc16 = crcmod.mkCrcFun(0x11021, rev=False, initCrc=0xFFFF, xorOut=0x0000)

async def send_command(client, command_id, payload):
    data = struct.pack('<H', command_id) + payload.ljust(16, b'\x00')
    crc = struct.pack('<H', crc16(data))
    packet = data + crc
    await client.write_gatt_char(COMMAND_CHAR_UUID, packet)
    print(f"Sent command {command_id} with payload {payload.hex()} and CRC {crc.hex()}")

async def send_firmware(client, file_path):
    with open(file_path, 'rb') as file:
        firmware = file.read()

    total_size = len(firmware)
    sector_size = 4096    
    mtu_size = 515

    # Start OTA
    await send_command(client, START_OTA_CMD, struct.pack('<I', total_size))

    for sector_index in range(0, total_size, sector_size):
        sector = firmware[sector_index:sector_index + sector_size]
        num_packets = (len(sector) + mtu_size - 4 - 1) // (mtu_size - 4)
        for packet_seq in range(num_packets):
            start = packet_seq * (mtu_size - 4)
            end = start + (mtu_size - 4)
            payload = sector[start:end]
            if packet_seq == num_packets - 1:  # Last packet of the sector
                packet_seq = 0xFF
                payload = payload.ljust(mtu_size - 6, b'\x00') + struct.pack('<H', crc16(sector))
            packet = struct.pack('<H', sector_index // sector_size) + struct.pack('<B', packet_seq) + payload
            await client.write_gatt_char(RECV_FW_CHAR_UUID, packet)
            print(f"Sent packet for sector {sector_index // sector_size}, packet_seq {packet_seq}")

    # Stop OTA
    await send_command(client, STOP_OTA_CMD, b'\x00' * 16)
    print("OTA update complete")

async def main():
    print("Scanning for devices...")
    devices = await BleakScanner.discover()
    for device in devices:
        print(device)

    address = input("Enter the address of the ESP32 device: ")
    file_path = input("Enter the path to the firmware file to send: ")

    async with BleakClient(address) as client:
        print(f"Connected to {address}")
        await send_firmware(client, file_path)

if __name__ == "__main__":
    asyncio.run(main())

# import asyncio
# import struct
# import crcmod
# from bleak import BleakClient, BleakScanner

# # Define UUIDs
# DIS_SERVICE_UUID = "0000180a-0000-1000-8000-00805f9b34fb"
# OTA_SERVICE_UUID = "00008020-0000-1000-8000-00805f9b34fb"
# RECV_FW_CHAR_UUID = "00008020-0000-1000-8000-00805f9b34fb"
# PROGRESS_BAR_CHAR_UUID = "00008021-0000-1000-8000-00805f9b34fb"
# COMMAND_CHAR_UUID = "00008022-0000-1000-8000-00805f9b34fb"
# CUSTOMER_CHAR_UUID = "00008023-0000-1000-8000-00805f9b34fb"

# # CRC16 function
# crc16 = crcmod.mkCrcFun(0x11021, rev=False, initCrc=0xFFFF, xorOut=0x0000)

# async def send_command(client, command_id, payload):
#     data = struct.pack('<H', command_id) + payload.ljust(16, b'\x00')
#     crc = struct.pack('<H', crc16(data))
#     packet = data + crc
#     await client.write_gatt_char(COMMAND_CHAR_UUID, packet)
#     print(f"Sent command {command_id} with payload {payload.hex()} and CRC {crc.hex()}")

# async def send_firmware(client, file_path):
#     with open(file_path, 'rb') as file:
#         firmware = file.read()

#     total_size = len(firmware)
#     sector_size = 4096    
#     mtu_size = 515 #ESP32

#     # Start OTA
#     await send_command(client, 0x0001, struct.pack('<I', total_size))

#     for sector_index in range(0, total_size, sector_size):
#         sector = firmware[sector_index:sector_index + sector_size]
#         num_packets = (len(sector) + mtu_size - 4 - 1) // (mtu_size - 4)
#         for packet_seq in range(num_packets):
#             start = packet_seq * (mtu_size - 4)
#             end = start + (mtu_size - 4)
#             payload = sector[start:end]
#             if packet_seq == num_packets - 1:  # Last packet of the sector
#                 packet_seq = 0xFF
#                 payload = payload.ljust(mtu_size - 6, b'\x00') + struct.pack('<H', crc16(sector))
#             packet = struct.pack('<H', sector_index // sector_size) + struct.pack('<B', packet_seq) + payload
#             try:
#                 await client.write_gatt_char(RECV_FW_CHAR_UUID, packet)
#                 print(f"Sent packet for sector {sector_index // sector_size}, packet_seq {packet_seq}")
#             except Exception as e:
#                 print(f"Error sending packet {packet_seq} for sector {sector_index // sector_size}: {e}")
#                 await asyncio.sleep(1)  # Wait a bit before retrying
#                 await client.write_gatt_char(RECV_FW_CHAR_UUID, packet)  # Retry sending packet

#     # Stop OTA
#     await send_command(client, 0x0002, b'\x00' * 16)
#     print("OTA update complete")

# async def main():
#     print("Scanning for devices...")
#     devices = await BleakScanner.discover()
#     for device in devices:
#         print(device)

#     address = input("Enter the address of the ESP32 device: ")
#     file_path = input("Enter the path to the firmware file to send: ")

#     async with BleakClient(address) as client:
#         print(f"Connected to {address}")
#         await send_firmware(client, file_path)

# if __name__ == "__main__":
#     asyncio.run(main())
