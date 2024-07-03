// SPDX-FileCopyrightText: 2011 Limor Fried for Adafruit Industries
// SPDX-FileCopyrightText: 2012 Tom Igoe
// SPDX-FileCopyrightText: 2018 Anne Barela for Adafruit Industries
//
// SPDX-License-Identifier: MIT

/*
  SD card test for WIZ5500 Compatible Ethernet Boards

 This example shows how use the utility libraries on which the'
 SD library is based in order to get info about your SD card.
 Very useful for testing a card when you're not sure whether its working or not.

 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno and Adafruit Metro
 ** MISO - pin 12 on Arduino Uno and Adafruit Metro
 ** CLK - pin 13 on Arduino Uno and Adafruit Metro
 ** CS - depends on your SD card shield or module (see below)

 created  28 Mar 2011 by Limor Fried
 modified 9 Apr 2012 by Tom Igoe
 modified 12 Apr 2018 by Anne Barela
 */
// include the SD library:
#include <SPI.h>
#include <SD.h>

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit #2971 W5500 by Seeed Studio: Pin 4
// Sparkfun SD shield: pin 8
// Arduino Mega: Pin 53
// MKRZero SD: SDCARD_SS_PIN
const int chipSelect = 4;
//
// Chip Select for W5500 Ethernet (must be set as output in initialization)
const int W5500_SS = 10;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(19200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("\nInitializing SD card...");

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  pinMode(W5500_SS, OUTPUT);     // set the Ethernet SS pin as an output (necessary!)
  digitalWrite(W5500_SS, HIGH);  // but turn off the W5500 chip for now  
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  // print the type of card
  Serial.print("\nCard type: ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                         // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);

  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
}

void loop(void) {
}