// It is designed to work with the other example Arduino9x_TX
 
#include <SPI.h>
// #include <RH_RF95.h>
 
 #define RFM95_CS 10
 #define RFM95_RST 9
 #define RFM95_INT 2
 
// Change to 434.0 or other frequency, must match RX's freq!
 #define RF95_FREQ 868.0
 
// Singleton instance of the radio driver
// RH_RF95 rf95(RFM95_CS, RFM95_INT);

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(9600);
  delay(100);
}

// the loop function runs over and over again forever
void loop() {
  Serial.print("test");   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
 // digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
