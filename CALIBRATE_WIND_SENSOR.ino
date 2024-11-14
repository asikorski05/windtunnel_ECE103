/* Calibrating and testing the wind speed sensor.
   Modified by Alex Sikorski for ECE103. Revision 2.
   Code modified from https://github.com/moderndevice/Wind_Sensor/blob/master/WindSensor/WindSensor.ino

   To calibrate, place a glass or cup over the sensor to ensure no wind movement.
   The sensor should not be touching any surface. Change the calibration factor
   until the sensor reads zero. 
*/
#include <LiquidCrystal.h>

// Wind sensor pins
const int analogPinForRV = 1;
const int analogPinForTMP = 0;
// LCD Pins
const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;

// Wind sensor adjustment
const float zeroWindAdjustment = 0.47; // negative numbers yield smaller wind speeds and vice versa.

// Vars for calculation
int TMP_Therm_ADunits;  //temp termistor value from wind sensor
float RV_Wind_ADunits;    //RV output from wind sensor 
float RV_Wind_Volts;
unsigned long lastMillis;
int TempCtimes100;
float zeroWind_ADunits;
float zeroWind_volts;
float WindSpeed_MPH;

// Initialize LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup()
{
  // Create LCD Matrix
  lcd.begin(16,2);
  lcd.print("Start!");

  // Start serial monitor for extended data
  Serial.begin(57600);
  Serial.println("Start!");

  delay(100);
  lcd.clear();
}

void loop() {

  if (millis() - lastMillis > 200)  // read every 200 ms
  {     
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
   
   // Serial Printing
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
    lastMillis = millis(); 

    // LCD Printing
    /* _________________
       |Drag  Lift  Wind|
       |0.00  0.00  00.0|
       ------------------
    */
    lcd.home();
    lcd.print("Drag  Lift  Wind");
    lcd.setCursor(0,1);
    lcd.print(0.00);
    lcd.setCursor(6,1);
    lcd.print(0.00);
    lcd.setCursor(12,1);
    lcd.print(WindSpeed_MPH);    
  } 
}


