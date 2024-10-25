/* This script sets up and records data from two load sensors with the
   purpose of measuring data from a wind tunnel apparatus.
   Before using this script, please make sure each sensor is calibrated.
   A thermal anemometer is used to measure wind speed.
   Data is output to the serial monitor at specified baud rate (57200).

   Written by Alex Sikorski for ECE103. Revision 1.
   Windspeed code modified from https://github.com/moderndevice/Wind_Sensor/blob/master/WindSensor/WindSensor.ino
   HX711 Library used from: https://github.com/bogde/HX711/releases
*/
#include <Arduino.h>
#include "HX711.h"
#include <Pushbutton.h>

// Insert these numbers from the calibration script.
// This ensures that each reading is accurate.
const int DRAG_CALIBRATION_SCALE = 0;
const int LIFT_CALIBRATION_SCALE = 0;
//const float THERM_WIND_CALIBRATION = 0.2;

// Circuit wiring
const int DRAG_DOUT_PIN = 2;
const int DRAG_SCK_PIN = 3;     // PWM
const int LIFT_DOUT_PIN = 4;
const int LIFT_SCK_PIN = 5;     // PWM
const int TARE_BUTTON_PIN = 7;  // Connected to 10K pulldown resistor
//#define RV_ANALOG_PIN = 1;      // Analog
//#define TMP_ANALOG_PIN = 0;     // Analog

/* WIND SPEED VARIABLES FOR CALCULATION
  int TMP_Therm_ADunits;  //temp termistor value from wind sensor
  float RV_Wind_ADunits;    //RV output from wind sensor 
  float RV_Wind_Volts;
  unsigned long lastMillis;
  int TempCtimes100;
  float zeroWind_ADunits;
  float zeroWind_volts;
  float WindSpeed_MPH;
*/

// Initializing instances of library
  HX711 drag;
  HX711 lift;

// Initilize Button
  Pushbutton tareButton(TARE_BUTTON_PIN);

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

/* Resetting analog pins for thermistor
  pinMode(A2, INPUT);        // GND pin      
  pinMode(A3, INPUT);        // VCC pin
  digitalWrite(A3, LOW);     // turn off pullups
*/

  Serial.print("Setup Complete! /n /n");
}

void loop() {

  // If button is pressed, tare the sensors.
  if (tareButton.getSingleDebouncedPress())
  {
    Serial.println("Taring both sensors...");
    drag.tare();
    lift.tare();
    delay(1000);
  }

  /* Reading both sensors and printing output.
     The 'if' statement allows us to safely continue
     execution if a hardware failure occurs.
     We do have to delay each reading to check
     sensor status however. (200ms)       
  */
  if ((lift.wait_ready_timeout(200)) && (drag.wait_ready_timeout(200)))
  {
    long L_reading = lift.get_units(10);
    Serial.print("Lift: ");
    Serial.println(L_reading, 2);

    long D_reading = drag.get_units(10);
    Serial.print("Drag: ");
    Serial.println(D_reading, 2);   
  }
  else
  {
    // Hardware failure, sensors did not initialize in time
    Serial.println("HX711 not found. Please check connections.");
    delay(5000);
  }

/* Wind sensor readings for manual calibration. Code modified from repository cited.
  if (millis() - lastMillis > 200)      // read every 200 ms - printing slows this down further
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
   
    Serial.print("Wind Speed: ");
    Serial.print((float)WindSpeed_MPH);
    Serial.println(" MPH")

    lastMillis = millis();
  }
*/

}