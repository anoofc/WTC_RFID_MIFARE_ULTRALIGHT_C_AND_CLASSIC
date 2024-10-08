#define DEBUG 0           // 1 For Debugging, 0 For No Debugging
#define MODE  1           // 0 For Slave, 1 For Master, 2 For Clear Data

#define DEVICE_ID  4      // Device ID for the device

#define SS_PIN          D8    
#define RST_PIN         D0

#include <Arduino.h>        // Include the Arduino library
#include <SPI.h>            // Library for SPI communication
#include <MFRC522.h>        // Library for RFID

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
MFRC522::StatusCode status;        //variable to get card status
MFRC522::MIFARE_Key key;          //create a MIFARE_Key struct named 'key', which will hold the card information

byte buffer_UL[18];               //data transfer buffer_UL (16+2 bytes data+CRC)
byte buffer[18];                  //data transfer buffer (16+2 bytes data+CRC)
byte size = sizeof(buffer);       //size of the buffer
byte size_UL = sizeof(buffer_UL); //size of the buffer_UL

byte trailerBlock   = 7;     // Block address for sector trailer
byte blockAddr      = 4;     // Block address for Mifare Classic
uint8_t pageAddr_UL = 0x06;  // In this example we will write/read 16 bytes (page 6,7,8 and 9).
                             // Ultraligth mem = 16 pages. 4 bytes per page. 
                             // Pages 0 to 4 are for special functions.  
uint8_t addData [] = {          // Data to write
    0x00, 0x00, 0x00, 0x00, // 0,  1,  2,  3
    0x00, 0x00, 0x00, 0x00, // 4,  5,  6,  7
    0x00, 0x00, 0x00, 0x00, // 8,  9,  10, 11
    0x00, 0x00, 0x00, 0x00  // 12, 13, 14, 15
};         
uint8_t readData [] = {         // Data read
    0x00, 0x00, 0x00, 0x00, // 0,  1,  2,  3
    0x00, 0x00, 0x00, 0x00, // 4,  5,  6,  7
    0x00, 0x00, 0x00, 0x00, // 8,  9,  10, 11
    0x00, 0x00, 0x00, 0x00  // 12, 13, 14, 15
};
uint8_t deviceID [] = {         // Device ID
    0x00, 0x00, 0x00, 0x00, // 0,  1,  2,  3
    0x00, 0x00, 0x00, 0x00, // 4,  5,  6,  7
    0x00, 0x00, 0x00, 0x00, // 8,  9,  10, 11
    0x00, 0x00, 0x00, 0x00  // 12, 13, 14, 15
};

// setDeviceId() function sets the device ID by iterating through each byte of the deviceID array 
// and assigning a value based on the current index. If the index matches the DEVICE_ID constant, 
// the corresponding element in the array is set to 0x01, otherwise it is set to 0x00. 

void setDeviceId(){             // Set Device ID
  for (byte i = 0; i < 16; i++) {
    if (i==DEVICE_ID){          
        deviceID[i] = 0x01;
    } else {
        deviceID[i] = 0x00;
    }

    if (DEBUG){Serial.print(deviceID[i]); Serial.print(", ");}
  } 
  if (DEBUG){Serial.println();}
}

/* processDataArray() function
 * Process the data array by performing a bitwise OR operation between the deviceID and readData arrays.
 * This function iterates through each element of the deviceID and readData arrays and performs a bitwise OR operation
 * between the corresponding elements. The result is stored in the addData array.
 * If the DEBUG flag is enabled, the function also prints the contents of the addData array to the Serial monitor.
 */
void processDataArray() {
    for (byte i = 0; i < 16; i++) {
        addData[i] = deviceID[i] | readData[i];
    }
    if (DEBUG) {
      Serial.println(F("Data in block after processing:"));
    for (int i = 0; i < 16; i++) {
      Serial.print(addData[i] < 0x10 ? " 0" : " ");
      Serial.print(addData[i]);
    }
    Serial.println();
    }

}


/* clearArray() function
 * Clear the addData and readData arrays by setting all elements to 0x00.
 * This function iterates through each element of the addData and readData arrays and sets the value to 0x00.
 * If the DEBUG flag is enabled, the function also prints the contents of the addData and readData arrays to the Serial monitor.
 */

void clearArray(){
  for (int i=0; i<16; i++){
    addData[i] = 0x00;
    readData[i] = 0x00;
  }
  if (DEBUG){
    Serial.println("PROCESSED DATA");
    for(int i=0; i<16; i++){
      Serial.print(addData[i]); Serial.print(", ");
    }
    Serial.println();
    Serial.println("READ DATA");
    for(int i=0; i<16; i++){
      Serial.print(readData[i]); Serial.print(", ");
    }
    Serial.println();
  }
}


// FOR CLASSIC CARD
/*
* authenticate() function
* Authenticate the card using key A.
* This function authenticates the card using key A and the specified trailer block.
*/
void authenticate(){
    // Authenticate using key A
    if (DEBUG){ Serial.println(F("Authenticating using key A..."));}
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
}

// dump_byte_array() function
// Dump a byte array to the Serial monitor. 
// This function takes a byte array and its size as input and prints each element in hexadecimal format.

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}



/**
 * Reads data from a block in the MIFARE Classic RFID tag.
 * 
 * This function reads data from a specific block in the MIFARE Classic RFID tag.
 * It first checks if the DEBUG flag is enabled and prints a message indicating the block being read.
 * Then, it calls the MIFARE_Read function of the MFRC522 library to read the data from the block.
 * If the DEBUG flag is enabled, it prints the data read from the block using the dump_byte_array function.
 * Finally, it copies the read data into the readData array.
 */
void readBlock_Classic(){
  // Read data from the block
  if (DEBUG){
    Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
  }
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (DEBUG){Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
    dump_byte_array(buffer, 16); Serial.println();
    Serial.println();
  }
  for (byte i = 0; i < 16; i++) {
      readData[i] = buffer[i];
  }
  
}

/**
 * Writes data to a block in the MIFARE Classic RFID tag.
 * 
 * This function writes data to a specific block in the MIFARE Classic RFID tag.
 * It first checks if the DEBUG flag is enabled and prints a message indicating the block being written to.
 * Then, it calls the MIFARE_Write function of the MFRC522 library to write the data to the block.
 * If the DEBUG flag is enabled, it prints the data written to the block using the dump_byte_array function.
 */

void writeBlock_Classic(){
// Write data to the block
  Serial.print(F("Writing data into block ")); Serial.print(blockAddr);
  Serial.println(F(" ..."));
  dump_byte_array(addData, 16); Serial.println();
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, addData, 16);
  if (DEBUG && status != MFRC522::STATUS_OK){
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

}


// FOR ULTRALIGHT CARD


/**
 * Writes data to MIFARE Ultralight RFID tag.
 * 
 * This function writes data to the MIFARE Ultralight RFID tag by calling the MIFARE_Ultralight_Write() function in a loop.
 * Each iteration of the loop writes 4 bytes of data to a specific page address on the tag.
 * The data to be written should be stored in the 'addData' array.
 * The 'pageAddr_UL' variable determines the starting page address on the tag where the data will be written.
 * The 'pageAddr_UL' variable increments by 1 in each iteration of the loop to write data to the next page address.
 * The 'status' variable is used to store the status code returned by the MIFARE_Ultralight_Write() function.
 * If the write operation fails, the status code is printed to the serial monitor.
 */

void writeData_UL(){
  // memcpy(buffer_UL,addData,16);

  for (int i=0; i < 4; i++) {
    //data is writen in blocks of 4 bytes (4 bytes per page)
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Ultralight_Write(pageAddr_UL+i, &addData[i*4], 4);
  }
  if (status == MFRC522::STATUS_OK) {
    if (DEBUG){Serial.println(F("MIFARE_Ultralight_Write() OK "));}
      // LED BLINK FLAG ENABLE HERE
  } else if (DEBUG && status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Read() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
  }
}

/**
 * Reads data from the MIFARE Ultralight RFID tag.
 * 
 * This function reads data from the MIFARE Ultralight RFID tag by calling the MIFARE_Read function of the MFRC522 library.
 * It reads data from the specified page address on the tag and stores it in the buffer_UL array.
 * The size_UL variable is used to store the size of the buffer_UL array.
 * If the read operation is successful, the data is copied into the readData array.
 * If the DEBUG flag is enabled, the function prints the read data to the Serial monitor using the dump_byte_array function.
 */ 

void readData_UL(){
  if (DEBUG){ Serial.println(F("Reading data ... ")); }
  //data in 4 block is readed at once.
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(pageAddr_UL, buffer_UL, &size_UL);
  if (DEBUG && status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  if (DEBUG){
    Serial.print(F("Readed data: "));}
    //Dump a byte array to Serial
    for (byte i = 0; i < 16; i++) {
      readData[i] = buffer_UL[i];
      if (DEBUG){
      Serial.print(buffer_UL[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer_UL[i], HEX);  }
    
    }
    if (DEBUG){Serial.println();}
}

/**
 * Prints the count of Devices NFC tag has tapped.
 * Only for Master Mode
 * This function iterates through the `readData` array and counts the number of elements that have a value of 1. 
 * The count is then printed to the Serial monitor as a character ('A').
 */
void masterDataPrint (){
  char count = '@';
  for (byte i = 0; i < 16; i++) {
      if (readData[i]==1){ count++;}
  }
  Serial.println(count);
}

// CHECK CARD TYPE AND WRITE DATA TO CARD

/**
 * Checks the type of RFID card detected and performs corresponding actions based on the card type and mode.
 * 
 * The cardCheck function is responsible for determining the type of RFID card detected and executing the appropriate actions based on the card type and mode. 
 * It first retrieves the card type using the PICC_GetType function from the mfrc522 library. 
 * If the card type is MIFARE Ultralight, it checks the mode and performs the necessary operations such as reading, processing, and writing data. 
 * If the card type is MIFARE Classic 1K, it authenticates using key A and performs operations based on the mode, such as reading, processing, and writing blocks of data. 
 * If the card type is not compatible, it prints a message indicating that the card is not compatible and halts the PICC. 
 * 
 * @note The DEBUG and MODE variables are assumed to be defined and set appropriately before calling this function.
 */
void cardCheck (){

  byte cardType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  if (DEBUG){ Serial.println("Card detected: " + String(cardType)); }
  // Check for compatibility with Mifare Ultralight
  if (cardType == MFRC522::PICC_TYPE_MIFARE_UL) {
    if (DEBUG){ Serial.println(F("MIFARE Ultralight"));}
    if (MODE == 0){           // SLAVE MODE
      readData_UL();
      processDataArray();
      writeData_UL();
      clearArray();
    } else if (MODE == 1){    // MASTER MODE
      readData_UL();
      masterDataPrint();
      clearArray();
      
    } else if (MODE == 2){    // CLEAR DATA MODE
      clearArray();
      writeData_UL();
    }


  } else if (cardType == MFRC522::PICC_TYPE_MIFARE_1K) {
    if (DEBUG){ Serial.println(F("MIFARE Classic 1K"));}
    authenticate();           // Authenticate using key A
    
    if (MODE == 0){           // SLAVE MODE
      readBlock_Classic();
      processDataArray();
      writeBlock_Classic();
      clearArray();
    } else if (MODE == 1){    // MASTER MODE
      readBlock_Classic();
      masterDataPrint();
      clearArray();
    } else if (MODE == 2){    // CLEAR DATA MODE
      clearArray();
      writeBlock_Classic();
    }

    mfrc522.PICC_HaltA();     // Halt PICC 

    // to do

  } else {
    Serial.println(F("Card is not compatible!!!"));
    mfrc522.PICC_HaltA();   // Halt PICC
    return;
  }

}

// SETUP 
void setup() {
  Serial.begin(9600);                   // Initialize serial communications with the PC
  SPI.begin();                          // Init SPI bus
  mfrc522.PCD_Init();                   // Init MFRC522 card 
  if (MODE==0) { setDeviceId(); }       // Set Device ID
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  if (DEBUG) {Serial.println(F("Initializing....."));}    
} 

void loop() {
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())   { return; }   
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) { return; }
  
  cardCheck();                // Check card type and write data to card 

  mfrc522.PICC_HaltA();       // Halt PICC 
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD

}