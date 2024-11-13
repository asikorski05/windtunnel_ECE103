/* This script sets up and records data from two load sensors with the
   purpose of measuring data from a wind tunnel apparatus.
   A thermal anemometer is used to measure wind speed.
   Before using this script, please make sure each sensor is calibrated.
   Data is output to the serial monitor at specified baud rate (57200).

   Written by Alex Sikorski for ECE103. Revision 4.
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

// Insert these numbers from the calibration script.
// Also insert the units used from the calculation.
const int DRAG_CALIBRATION_SCALE = 797.125;  // A
const int LIFT_CALIBRATION_SCALE = 783.125;  // B
char UNITS[] = "grams";
const float THERM_WIND_CALIBRATION = -2.4;

// define wiring and vars
const int DRAG_DOUT_PIN = 2;
const int DRAG_SCK_PIN = 3;
const int LIFT_DOUT_PIN = 4;
const int LIFT_SCK_PIN = 5;
const int TARE_BUTTON_PIN = 7;  // Pulled high when pressed
const int SLOW_BUTTON_PIN = 8;
const int RV_ANALOG_PIN = 1;
const int TMP_ANALOG_PIN = 0; 
bool slow = false;

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
  Pushbutton slowButton(SLOW_BUTTON_PIN);

void setup() {

// Start Serial Monitor with 57600 baud rate
  Serial.begin(57600);

// Initialize the load sensors
  drag.begin(DRAG_DOUT_PIN, DRAG_SCK_PIN);  // Initialize hardware
  drag.set_scale(DRAG_CALIBRATION_SCALE);   // Set calibration
  drag.tare();                              // Reset the scale to 0

  lift.begin(LIFT_DOUT_PIN, LIFT_SCK_PIN);  // Repeat for 2nd scale
  lift.set_scale(LIFT_CALIBRATION_SCALE);
  lift.tare();

  Serial.print("\n\n Setup Complete! \n\n");
  delay(1000);
}

void loop() {     // MAIN PROGRAM LOOP

  // If button is pressed, tare the sensors.
  if (tareButton.getSingleDebouncedPress())
  {
    Serial.println("Taring sensors...");
    drag.tare();
    lift.tare();
    delay(1000);
  }

  /* Reading both sensors and printing output.
     The 'if' statement allows us to safely continue
     execution if a hardware failure occurs.       
  */  
  if (lift.wait_ready_timeout(200) && drag.wait_ready_timeout(200))
  {
    Serial.print("Drag:\t");
    Serial.print(drag.get_units(), 1);
    Serial.print("  ");
    Serial.print(UNITS);
    Serial.print("\t\tLift:\t");
    Serial.print(drag.get_units(), 1);
    Serial.print("  ");
    Serial.print(UNITS);  
  }
  else
  {
    // Hardware failure, sensors did not initialize in time
    Serial.println("HX711 not found. Please check connections.");
    delay(5000);
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
   
    Serial.print("Wind Speed: ");
    Serial.print((float)WindSpeed_MPH);
    Serial.print(" MPH");

  // delay print output while slow mode is on;
  // only needed for serial monitor
  if(slow)
  {
    delay(250);
  }

  Serial.print("\n"); // print new line for next output
}