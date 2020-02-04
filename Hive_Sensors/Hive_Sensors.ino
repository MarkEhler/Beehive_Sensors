// read air pollution sensors and print data to serial monitor
// print data to sd card , deliminated
// read data and RTC

#include <SD.h>

#define Asense1 A1
#define Asense2 A2
#define Asense3 A3
const int Ro = 330;
//ppm formula
//Pull-Down Resistance
const int RLOAD1 = 1;
const int RLOAD10 = 10;
//resistance in open air

// Define timeout in ms
#define TIMEOUT 3000


// Alarmlevels to be modified
#define TEMPALARM 30
#define HUMALARM 65
#define CO2ALARM 1000

// Set the low, mid and high thresolds for flagging data?
// T in deg Celsius
//float tempthreshold[3] = {
// 15, 20, 25};
// CO2 in ppm
//float airthreshold[3] = {
//  200, 200, 300};
// Rel Humidity in percent
//float humthreshold[3] = {
//  35, 45, 60};

/*
// Initialize the gas Sensor
MQ135 gasSensori = MQ135(Asense1);
MQ135 gasSensorii = MQ135(Asense2);
MQ135 gasSensoriii = MQ135(Asense3);
*/

void setup() {
  pinMode(Asense1,INPUT);
  pinMode(Asense2,INPUT);
  pinMode(Asense3,INPUT);
  //Set up the serial terminal
  Serial.begin(9600); 

  int Row[] = {};
  
  

}
/*
void loop() {
  // Wait a few seconds between measurements.
  // TODO: This is not ideal - better put the Arduino to sleep here
  delay(TIMEOUT);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit - in case you still don't use the SI
  //float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
*/
  float getResistance() {
  int val = analogRead(A2);
  return ((1023. / (float)val) - 1.) * RLOAD10;
}
void loop() {
  
  // after burnin time sensors need 3 minutes before readings are stable
  delay(3000); //*60
 float Vrl = 5.0 * analogRead(Asense2) / 1023;
 float Rs = 1 * ( 5.0 - Vrl) / Vrl;
 int ratio = 116.6 * pow((Rs/Ro), -2.76);
// ratio = constrain(ratio, 0, 30);  // 400 .. 5000
 Serial.print ( "CO : ");
 Serial.println(Rs);
 Serial.println(ratio);

/*
//  Calculate resistance  set with R0

  int co2nowi[10];
  int co2nowii[10];
  int co2nowiii[10];//int array for co2 readings
  int co2rawi = 0;                               //int for raw value of co2
  int co2rawii = 0;
  int co2rawiii = 0;
  int co2compi = 0;
  int co2compii = 0;
  int co2compiii = 0;//int for compensated co2 
  int co2ppm = 0;                               //int for calculated ppm
  int z = 0;
  int zz = 0;
  int zzz = 0;                                  //int for averaging

  for (int x = 0;x<10;x++){                   //samplpe co2 10x over 2 seconds
  co2nowi[x]=analogRead(Asense1);
  co2nowii[x]=analogRead(Asense1);
  co2nowiii[x]=analogRead(Asense1);
  delay(200);
}

  for (int x = 0;x<10;x++){                     //add samples together
      z=z + co2nowi[x];
      zz=zz + co2nowii[x];
      zzz=zzz + co2nowiii[x];
    }
    co2rawi = z/10; 
    co2rawii = zz/10;
    co2rawiii = zzz/10;//divide samples by 10
    co2compi = co2rawi - Ro;
    co2compii = co2rawii - Ro;
    co2compiii = co2rawiii - Ro;
    //get compensated value
//    co2ppm = map(co2comp,0,1023,400,5000);    


  Serial.print("T: "); 
  Serial.print(t);
  Serial.print(" *C\t");
  Serial.print("H: "); 
  Serial.print(h);
  Serial.print(" %\t");


  Serial.print("Sensor 1 RZero: ");
  Serial.print(co2compi);
  Serial.println(" kOhm");
  Serial.println("#####");
  Serial.print("Sensor 1 RZero: ");
  Serial.print(co2compii);
  Serial.println(" kOhm");
  Serial.println("#####");
  Serial.print("Sensor 1 RZero: ");
  Serial.print(co2compiii);
  Serial.println(" kOhm");
  */

}
