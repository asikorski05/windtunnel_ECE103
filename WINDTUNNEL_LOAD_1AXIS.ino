/* This script sets up and records data from one load sensor with the
   purpose of measuring data from a wind tunnel apparatus.
   Before using this script, please make sure each sensor is calibrated.
   Data is output to the serial monitor at specified baud rate (57200).

   Written by Alex Sikorski for ECE103. Revision 3.
   Windspeed code modified from https://github.com/moderndevice/Wind_Sensor/blob/master/WindSensor/WindSensor.ino
   HX711 Library used from: https://github.com/bogde/HX711/releases
*/

#include <Arduino.h>
#include "HX711.h"
#include <Pushbutton.h>

// Insert these numbers from the calibration script
const int DRAG_CALIBRATION_SCALE = 400;
const int LIFT_CALIBRATION_SCALE = 400;
char UNITS[] = "grams";

// define wiring
const int DRAG_DOUT_PIN = 2;
const int DRAG_SCK_PIN = 3;
const int LIFT_DOUT_PIN = 4;
const int LIFT_SCK_PIN = 5;
const int TARE_BUTTON_PIN = 7;  // Connected to 10K pulldown resistor

// Initializing libraries as objects
  HX711 drag;
  //HX711 lift;
  Pushbutton tareButton(TARE_BUTTON_PIN);

void setup() {

  // Start Serial Monitor with 57600 baud rate
  Serial.begin(57600);

  // Initialize the load sensors
  drag.begin(DRAG_DOUT_PIN, DRAG_SCK_PIN);  // Initialize hardware
  drag.set_scale(DRAG_CALIBRATION_SCALE);   // Set calibration
  drag.tare();                              // Reset the scale to 0

  //lift.begin(LIFT_DOUT_PIN, LIFT_SCK_PIN);  // Repeat for 2nd scale
  //lift.set_scale(LIFT_CALIBRATION_SCALE);
  //lift.tare();

  Serial.print("\n \n Setup Complete! \n \n");
  delay(1000);
}

void loop() {

  // If button is pressed, tare the sensors.
  if (tareButton.getSingleDebouncedPress())
  {
    Serial.println("Taring sensor...");
    drag.tare();
    //lift.tare();
    delay(1000);
  }

  /* Reading both sensors and printing output.
     The 'if' statement allows us to safely continue
     execution if a hardware failure occurs. (200ms delay)       
  */
  if (drag.wait_ready_timeout(200))
  {
    Serial.print("Drag:\t");
    Serial.print(drag.get_units(), 1);
    Serial.print("  ");
    Serial.println(UNITS);
    //Serial.print("\t\tLift:\t");
    //Serial.print(drag.get_units(), 1);
    //Serial.print("  ");
    //Serial.println(UNITS);  
  }
  else
  {
    // Hardware failure, sensors did not initialize in time
    Serial.println("HX711 not found. Please check connections.");
    delay(5000);
  }
}