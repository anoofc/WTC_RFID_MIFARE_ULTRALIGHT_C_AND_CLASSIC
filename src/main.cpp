#define DEBUG 1
#define MODE  0          // 0 For Slave, 1 For Master, 2 For Clear Data


#define DEVICE_ID  4      // Device ID for the device

#define SS_PIN          D8
#define RST_PIN         D0

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
MFRC522::StatusCode status;        //variable to get card status
MFRC522::MIFARE_Key key;          //create a MIFARE_Key struct named 'key', which will hold the card information

byte buffer_UL[18];           //data transfer buffer_UL (16+2 bytes data+CRC)
byte buffer[18];              //data transfer buffer (16+2 bytes data+CRC)
byte size = sizeof(buffer);   //size of the buffer
byte size_UL = sizeof(buffer_UL); //size of the buffer_UL

byte trailerBlock   = 7;
byte blockAddr      = 4;     // Block address for Mifare Classic
uint8_t pageAddr_UL = 0x06;  // In this example we will write/read 16 bytes (page 6,7,8 and 9).
                             // Ultraligth mem = 16 pages. 4 bytes per page. 
                             // Pages 0 to 4 are for special functions.  
uint8_t addData [] = {
    0x00, 0x00, 0x00, 0x00, // 0,  1,  2,  3
    0x00, 0x00, 0x00, 0x00, // 4,  5,  6,  7
    0x00, 0x00, 0x00, 0x00, // 8,  9,  10, 11
    0x00, 0x00, 0x00, 0x00  // 12, 13, 14, 15
};         
uint8_t readData [] = {
    0x00, 0x00, 0x00, 0x00, // 0,  1,  2,  3
    0x00, 0x00, 0x00, 0x00, // 4,  5,  6,  7
    0x00, 0x00, 0x00, 0x00, // 8,  9,  10, 11
    0x00, 0x00, 0x00, 0x00  // 12, 13, 14, 15
};
uint8_t deviceID [] = {
    0x00, 0x00, 0x00, 0x00, // 0,  1,  2,  3
    0x00, 0x00, 0x00, 0x00, // 4,  5,  6,  7
    0x00, 0x00, 0x00, 0x00, // 8,  9,  10, 11
    0x00, 0x00, 0x00, 0x00  // 12, 13, 14, 15
};

void setDeviceId(){
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


void clearArray(){
  for (int i=0; i<16; i++){
    addData[i] = 0x00;
    readData[i] = 0x00;
  }
    if (DEBUG){
      Serial.println("SUM DATA");
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

void authenticate(){
    // Authenticate using key A
    if (DEBUG){ Serial.println(F("Authenticating using key A..."));}
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

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

// void processDataArray_UL(){
//   for (byte i = 0; i < 16; i++) {
//     if (readData[i] == 0){ 
//       addData[i] = deviceID[i];
//     } else {
//       addData[i] = readData[i];
//     }    
//   }
//   if (DEBUG) {
//     Serial.println(F("Data in block after processing:"));
//     for (int i = 0; i < 16; i++) {
//       Serial.print(addData[i] < 0x10 ? " 0" : " ");
//       Serial.print(addData[i]);
//     }
//     Serial.println();
//   }
// }

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


void readData_UL(){
  if (DEBUG){ Serial.println(F("Reading data ... ")); }
  //data in 4 block is readed at once.
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(pageAddr_UL, buffer_UL, &size_UL);
  if (DEBUG && status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  Serial.print(F("Readed data: "));
  //Dump a byte array to Serial
  for (byte i = 0; i < 16; i++) {
    readData[i] = buffer_UL[i];
    Serial.print(buffer_UL[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer_UL[i], HEX);
  
  }
  Serial.println();
}

void masterDataPrint (){
  char count = '@';
  for (byte i = 0; i < 16; i++) {
    
      addData[i] = buffer[i];
      if (addData[i]==1){ count++;}
  }
  Serial.println(count);
}

// CHECK CARD TYPE AND WRITE DATA TO CARD

void cardCheck (){

  byte cardType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  if (DEBUG){ Serial.println("Card detected: " + String(cardType)); }
  // Check for compatibility with Mifare Ultralight
  if (cardType == MFRC522::PICC_TYPE_MIFARE_UL) {
    if (DEBUG){ Serial.println(F("MIFARE Ultralight"));}
    if (MODE == 0){       // SLAVE MODE
      readData_UL();
      processDataArray();
      writeData_UL();
    } else if (MODE == 1){    // MASTER MODE
      readData_UL();
      masterDataPrint();
      
    } else if (MODE == 2){    // CLEAR DATA
      clearArray();
      writeData_UL();
    }


  } else if (cardType == MFRC522::PICC_TYPE_MIFARE_1K) {
    if (DEBUG){ Serial.println(F("MIFARE Classic 1K"));}
    authenticate();           // Authenticate using key A
    
    if (MODE == 0){ 
      readBlock_Classic();
      processDataArray();
      writeBlock_Classic();
    } else if (MODE == 1){
      readBlock_Classic();
      masterDataPrint();
    } else if (MODE == 2){
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



void setup() {
  Serial.begin(9600);       // Initialize serial communications with the PC
  SPI.begin();              // Init SPI bus
  mfrc522.PCD_Init();       // Init MFRC522 card 
  if (MODE==0) { setDeviceId(); }            // Set Device ID
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

  
  cardCheck();        // Check card type and write data to card 

  clearArray();

  mfrc522.PICC_HaltA();     // Halt PICC 
  mfrc522.PCD_StopCrypto1();

}