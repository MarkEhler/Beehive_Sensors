/*
 * Rui Santos 
 * Complete Project Details http://randomnerdtutorials.com
 */

#include <SPI.h> //for the SD card module
#include <SD.h> // for the SD card
//#include <DHT.h> // for the DHT sensor
#include <RTClib.h> // for the RTC

int const chipSelect = 10; 
int const interruptPin = 2;

// Create a file to store the data
File myFile;

// RTC
RTC_DS1307 rtc;


void setup() {

  //initializing Serial monitor
  Serial.begin(9600);

  bool parse = false;
  bool config = false;
  // setup for the RTC
  rtc.begin();
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
  if(!rtc.isrunning()) {
    Serial.println("RTC is NOT running...setting time");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
    
  // setup for the SD card
  Serial.print("Initializing SD card...");

  if(!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
    
  //open file
  myFile=SD.open("DATA.txt", FILE_WRITE);

  // if the file opened ok, write to it:
  if (myFile) {
    Serial.println("File opened ok");
    // print the headings for our data
    myFile.println("Date,Time,Temperature ºC");
  }
  myFile.close();
}

void loggingTime() {
  DateTime now = rtc.now();
  myFile = SD.open("DATA.txt", FILE_WRITE);
  if (myFile) {
    myFile.print(now.year(), DEC);
    myFile.print('/');
    myFile.print(now.month(), DEC);
    myFile.print('/');
    myFile.print(now.day(), DEC);
    myFile.print(',');
    myFile.print(now.hour(), DEC);
    myFile.print(':');
    myFile.print(now.minute(), DEC);
    myFile.print(':');
    myFile.print(now.second(), DEC);
    myFile.print(",");
  }
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.println(now.day(), DEC);
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);
  myFile.close();
  delay(1000);  
}
  
  

void loop() {
  loggingTime();
  delay(5000);
}
