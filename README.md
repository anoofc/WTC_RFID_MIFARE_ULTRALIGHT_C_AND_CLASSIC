# RFID Reader and Writer

This project is an RFID reader and writer using an Arduino and the MFRC522 RFID module. The code allows the Arduino to read and write data to MIFARE Classic and MIFARE Ultralight RFID tags. The project supports different modes of operation, including Master, Slave, and Clear Data modes.

## Features

- **Read and Write Data**: Supports reading and writing data to MIFARE Classic and MIFARE Ultralight RFID tags.
- **Modes of Operation**:
  - **Master Mode**: Reads data from the RFID tag and prints the count of devices that have tapped the tag.
  - **Slave Mode**: Reads data from the RFID tag, processes it, and writes the processed data back to the tag.
  - **Clear Data Mode**: Clears the data on the RFID tag.
- **Debugging**: Optional debugging output to the Serial monitor.

## Hardware Requirements

- WeMos D1 Mini & R2 (Or any Arduino Supported Microcontroller)
- MFRC522 RFID module
- RFID tags (MIFARE Classic and MIFARE Ultralight)
- Connecting wires

## Pin Configuration

- `SS_PIN`: D8
- `RST_PIN`: D0

## Software Requirements

- VS Code with PlatformIO Extension / Arduino IDE
- MFRC522 library
- SPI library

## Installation

1. **Install the Arduino IDE**: Download and install the Arduino IDE from the [official website](https://www.arduino.cc/en/software).
2. **Install the MFRC522 Library**: Open the Arduino IDE, go to `Sketch` > `Include Library` > `Manage Libraries`, and search for "MFRC522". Install the library.
3. **Install the SPI Library**: The SPI library is usually pre-installed with the Arduino IDE. If not, follow the same steps as above to install it.

## Usage

1. **Connect the Hardware**: Connect the MFRC522 RFID module to the Arduino as per the pin configuration mentioned above.
2. **Upload the Code**: Open the provided code in the Arduino IDE and upload it to the Arduino board.
3. **Open Serial Monitor**: Open the Serial Monitor from the Arduino IDE to view debugging information and interact with the RFID tags.

## Code Overview

### Constants and Variables

- `DEBUG`: Set to `1` for debugging output, `0` for no debugging.
- `MODE`: Set to `0` for Slave mode, `1` for Master mode, `2` for Clear Data mode.
- `DEVICE_ID`: Device ID for the device.
- `SS_PIN`, `RST_PIN`: Pin configuration for the MFRC522 module.
- `MFRC522 mfrc522`: Instance of the MFRC522 class.
- `MFRC522::StatusCode status`: Variable to get card status.
- `MFRC522::MIFARE_Key key`: Struct to hold the card information.
- `byte buffer_UL[18]`, `byte buffer[18]`: Data transfer buffers.
- `byte size`, `byte size_UL`: Sizes of the buffers.
- `byte trailerBlock`, `byte blockAddr`, `uint8_t pageAddr_UL`: Block and page addresses for reading/writing data.
- `uint8_t addData[16]`, `uint8_t readData[16]`, `uint8_t deviceID[16]`: Arrays to hold data to write, read data, and device ID.

### Functions

- `setDeviceId()`: Sets the device ID.
- `processDataArray()`: Processes the data array by performing a bitwise OR operation between the `deviceID` and `readData` arrays.
- `clearArray()`: Clears the `addData` and `readData` arrays.
- `authenticate()`: Authenticates the card using key A.
- `dump_byte_array(byte *buffer, byte bufferSize)`: Dumps a byte array to the Serial monitor.
- `readBlock_Classic()`: Reads data from a block in the MIFARE Classic RFID tag.
- `writeBlock_Classic()`: Writes data to a block in the MIFARE Classic RFID tag.
- `writeData_UL()`: Writes data to the MIFARE Ultralight RFID tag.
- `readData_UL()`: Reads data from the MIFARE Ultralight RFID tag.
- `masterDataPrint()`: Prints the count of devices that have tapped the RFID tag (only for Master mode).
- `cardCheck()`: Checks the type of RFID card detected and performs corresponding actions based on the card type and mode.

### Setup and Loop

- `setup()`: Initializes the Serial communication, SPI bus, and MFRC522 module. Sets the device ID if in Slave mode.
- `loop()`: Continuously looks for new cards, selects one of the cards, and checks the card type to perform the appropriate actions.

## Authors

- [@anoofc](https://www.github.com/anoofc)


## 🔗 Let's Connect
[![linkedin](https://img.shields.io/badge/linkedin-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/anoofc)
[![twitter](https://img.shields.io/badge/twitter-1DA1F2?style=for-the-badge&logo=twitter&logoColor=white)](https://twitter.com/anoofc)



## License