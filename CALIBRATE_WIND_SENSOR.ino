/* Calibrating the wind speed sensor.
   Modified by Alex Sikorski for ECE103. Revision 1.
   Code modified from https://github.com/moderndevice/Wind_Sensor/blob/master/WindSensor/WindSensor.ino

   To calibrate, place a glass or cup over the sensor to ensure no wind movement.
   The sensor should not be touching any surface. Change the calibration factor
   until the sensor reads zero. 
*/

#include <Arduino.h>

// adjust the zeroWindAdjustment until the sensor reads about zero with the glass over it.

const float zeroWindAdjustment =  -2.4; // negative numbers yield smaller wind speeds and vice versa.


// Define wiring
const int RV_ANALOG_PIN = 1;
const int TMP_ANALOG_PIN = 0;

// Vars for calculation
int TMP_Therm_ADunits;    //temp termistor value from wind sensor
float RV_Wind_ADunits;    //RV output from wind sensor 
float RV_Wind_Volts;
int TempCtimes100;
float zeroWind_ADunits;
float zeroWind_volts;
float WindSpeed_MPH;

void setup() {

  Serial.begin(57600);
  Serial.println("start");

  //   Uncomment the three lines below to reset the analog pins A2 & A3
  //   This is code from the Modern Device temp sensor (not required)
  //pinMode(A2, INPUT);        // GND pin      
  //pinMode(A3, INPUT);        // VCC pin
  //digitalWrite(A3, LOW);     // turn off pullups

}

void loop() {

    TMP_Therm_ADunits = analogRead(analogPinForTMP);
    RV_Wind_ADunits = analogRead(analogPinForRV);
    RV_Wind_Volts = (RV_Wind_ADunits *  0.0048828125);

    // these are all derived from regressions from raw data as such they depend on a lot of experimental factors
    // such as accuracy of temp sensors, and voltage at the actual wind sensor, (wire losses) which were unaccouted for.
    TempCtimes100 = (0.005 *((float)TMP_Therm_ADunits * (float)TMP_Therm_ADunits)) - (16.862 * (float)TMP_Therm_ADunits) + 9075.4;  

    zeroWind_ADunits = -0.0006*((float)TMP_Therm_ADunits * (float)TMP_Therm_ADunits) + 1.0727 * (float)TMP_Therm_ADunits + 47.172;  //  13.0C  553  482.39

    zeroWind_volts = (zeroWind_ADunits * 0.0048828125) - zeroWindAdjustment;  

    // This from a regression from data in the form of 
    // Vraw = V0 + b * WindSpeed ^ c
    // V0 is zero wind at a particular temperature
    // The constants b and c were determined by some Excel wrangling with the solver.
    
   WindSpeed_MPH =  pow(((RV_Wind_Volts - zeroWind_volts) /.2300) , 2.7265);   
   
    Serial.print("  TMP volts ");
    Serial.print(TMP_Therm_ADunits * 0.0048828125);
    
    Serial.print(" RV volts ");
    Serial.print((float)RV_Wind_Volts);

    Serial.print("\t  TempC*100 ");
    Serial.print(TempCtimes100 );

    Serial.print("   ZeroWind volts ");
    Serial.print(zeroWind_volts);

    Serial.print("   WindSpeed MPH ");
    Serial.println((float)WindSpeed_MPH);

}


