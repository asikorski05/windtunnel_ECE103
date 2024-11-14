/* This script sets up and records data from two load sensors with the
   purpose of measuring data from a wind tunnel apparatus.
   A thermal anemometer is used to measure wind speed.
   Before using this script, please make sure each sensor is calibrated.
   Data is output to the serial monitor at specified baud rate (57200).

   Written by Alex Sikorski for ECE103. Revision 5.
   Windspeed code modified from https://github.com/moderndevice/Wind_Sensor/blob/master/WindSensor/WindSensor.ino
   HX711 Library used from: https://github.com/bogde/HX711/releases

   Connect the drag sensor's DOUT to D2, and SCK to D3.  (Parallel to airflow)
   Connect the lift sensor's DOUT to D4, and SCK to D5.  (Orthogonal to airflow)
   Connect the thermistor's TMP to A0, and RV to A1.
   Pin 7 is connected to a pushbutton with a pulldown resistor.
*/
#include <Arduino.h>
#include "HX711.h"
#include <Pushbutton.h>
#include <LiquidCrystal.h>


// Insert these numbers from the calibration script.
// Also insert the units used from the calculation.
const int DRAG_CALIBRATION_SCALE = 797.125;  // A
const int LIFT_CALIBRATION_SCALE = 783.125;  // B
char UNITS[] = "grams";
const float THERM_WIND_CALIBRATION = 0.475;

// How many decimal places to print in the serial monitor?
// Default is 1 due to fluctuating values.
const int decimalPrecision = 1;


// define wiring and vars
  // Drag sensor
  const int DRAG_DOUT_PIN = 2, DRAG_SCK_PIN = 3;
  // Lift sensor
  const int LIFT_DOUT_PIN = 4, LIFT_SCK_PIN = 5;
  // Tare Button (Pulled low)
  const int TARE_BUTTON_PIN = 7;
  // Wind sensor
  const int RV_ANALOG_PIN = 1, TMP_ANALOG_PIN = 0;
  // LCD
  const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;


// WIND SPEED VARIABLES FOR CALCULATION
  int TMP_Therm_ADunits;  //temp thermistor value from wind sensor
  float RV_Wind_ADunits;  //RV output from wind sensor 
  float RV_Wind_Volts;
  unsigned long lastMillis;
  int TempCtimes100;
  float zeroWind_ADunits;
  float zeroWind_volts;
  float WindSpeed_MPH;

// Initializing libraries as objects
  HX711 drag;
  HX711 lift;
  Pushbutton tareButton(TARE_BUTTON_PIN);
  LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {

// Start Serial Monitor with 57600 baud rate
  Serial.begin(57600);

// Create LCD Matrix (col, row)
  lcd.begin(16,2);

// Initialize the load sensors
  drag.begin(DRAG_DOUT_PIN, DRAG_SCK_PIN);  // Initialize hardware
  drag.set_scale(DRAG_CALIBRATION_SCALE);   // Set calibration
  drag.tare();                              // Reset the scale to 0

  lift.begin(LIFT_DOUT_PIN, LIFT_SCK_PIN);  // Repeat for 2nd scale
  lift.set_scale(LIFT_CALIBRATION_SCALE);
  lift.tare();


  Serial.print("\n\n Setup Complete! \n\n");
  lcd.print("Setup Complete!");
  delay(500);
  lcd.clear();
}

void loop() {     // MAIN PROGRAM LOOP

  // If button is pressed, tare the sensors.
  if (tareButton.getSingleDebouncedPress())
  {
    Serial.println("Taring sensors...");
    lcd.clear();
    lcd.print("Taring Sensors..");

    drag.tare();
    lift.tare();

    delay(500);
    lcd.clear();
  }
  
  /* Reading both sensors and printing output.
     The 'if' statement allows us to continue
     execution if a hardware failure occurs.       
  */  
    if (lift.wait_ready_timeout(200) && drag.wait_ready_timeout(200))
    {
      // Serial out
      Serial.print("Drag:\t");
      Serial.print(drag.get_units(), decimalPrecision);
      Serial.print("  ");
      Serial.print(UNITS);
      Serial.print("\t\tLift:\t");
      Serial.print(lift.get_units(), decimalPrecision);
      Serial.print("  ");
      Serial.print(UNITS);
      Serial.print("\t");
      // LCD out
      lcd.home();                               // __________________
      lcd.print("Drag  Lift  Wind");            // |Drag  Lift  Wind|
      lcd.setCursor(0,1);                       // |0.00  0.00  0.00|
      lcd.print(drag.get_units(), 1);           // ------------------
      lcd.setCursor(4,1);                       // 16 character limit!
      lcd.print("  ");
      lcd.setCursor(6,1);
      lcd.print(lift.get_units(), 1);
      lcd.setCursor(10,1);
      lcd.print("  ");
    }
    else
    {
      // Hardware failure, sensors did not initialize in time
      Serial.println("HX711 not found. Please check connections.");
      lcd.clear();
      lcd.print("Sensor Error.");
      lcd.setCursor(0,1);   // Set to bottom row
      lcd.print("Check connection");
      delay(10000);
      lcd.clear();
    }

  // Wind sensor code modified from repository cited.
   
    TMP_Therm_ADunits = analogRead(TMP_ANALOG_PIN);
    RV_Wind_ADunits = analogRead(RV_ANALOG_PIN);
    RV_Wind_Volts = (RV_Wind_ADunits *  0.0048828125);

    // these are all derived from regressions from raw data as such they depend on a lot of experimental factors
    // such as accuracy of temp sensors, and voltage at the actual wind sensor, (wire losses) which were unaccouted for.
    TempCtimes100 = (0.005 *((float)TMP_Therm_ADunits * (float)TMP_Therm_ADunits)) - (16.862 * (float)TMP_Therm_ADunits) + 9075.4;  

    zeroWind_ADunits = -0.0006*((float)TMP_Therm_ADunits * (float)TMP_Therm_ADunits) + 1.0727 * (float)TMP_Therm_ADunits + 47.172;  //  13.0C  553  482.39

    zeroWind_volts = (zeroWind_ADunits * 0.0048828125) - THERM_WIND_CALIBRATION;  

    /* This from a regression from data in the form of 
       Vraw = V0 + b * WindSpeed ^ c
       V0 is zero wind at a particular temperature
       The constants b and c were determined by some Excel wrangling with the solver.
    */    
    WindSpeed_MPH =  pow(((RV_Wind_Volts - zeroWind_volts) /.2300) , 2.7265);   
   
    // Serial out
    Serial.print("Wind Speed: ");
    Serial.print((float)WindSpeed_MPH);
    Serial.print(" MPH\t");
    Serial.print("\tTemp Celsius*100: ");
    Serial.println(TempCtimes100);
    // LCD out
    lcd.setCursor(12,1);
    lcd.print(WindSpeed_MPH); 
 
}