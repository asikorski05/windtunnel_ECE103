/* This script sets up and records data from two load sensors with the
   purpose of measuring data from a wind tunnel apparatus.
   Before using this script, please make sure each sensor is calibrated.
   Data is output to the serial monitor at specified baud rate (57200).

   Written by Alex Sikorski for ECE103. Revision 4.
   Windspeed code modified from https://github.com/moderndevice/Wind_Sensor/blob/master/WindSensor/WindSensor.ino
   HX711 Library used from: https://github.com/bogde/HX711/releases
*/

#include <Arduino.h>
#include "HX711.h"
#include <Pushbutton.h>

// Insert these numbers from the calibration script
const int DRAG_CALIBRATION_SCALE = 797.125;  // A
const int LIFT_CALIBRATION_SCALE = 783.125; // B
char UNITS[] = "grams";

// How many decimal places to print?
// Default is 1 due to fluctuating values.
const int decimalPrecision = 1;

// define wiring

const int DRAG_DOUT_PIN = 2;  // A
const int DRAG_SCK_PIN = 3;
const int LIFT_DOUT_PIN = 4;  // B
const int LIFT_SCK_PIN = 5;
const int TARE_BUTTON_PIN = 7;
const int SLOW_BUTTON_PIN = 8;

bool slow = false;

// Initializing libraries as objects
  HX711 drag;
  HX711 lift;
  Pushbutton tareButton(TARE_BUTTON_PIN);
  Pushbutton slowButton(SLOW_BUTTON_PIN);

void setup() {
  Serial.begin(57600);

  // Initialize the load sensors
  drag.begin(DRAG_DOUT_PIN, DRAG_SCK_PIN);  // Initialize hardware
  drag.set_scale(DRAG_CALIBRATION_SCALE);   // Set calibration
  drag.tare();                              // Reset the scale to 0

  lift.begin(LIFT_DOUT_PIN, LIFT_SCK_PIN);  // Repeat for 2nd scale
  lift.set_scale(LIFT_CALIBRATION_SCALE);
  lift.tare();

  Serial.print("\n \n Setup Complete! \n \n");
  delay(1000);
}

void loop() {

  // If button is pressed, tare the sensors.
  if (tareButton.getSingleDebouncedPress())
  {
    Serial.println("\nTaring sensors...\n");
    drag.tare();
    lift.tare();
    delay(1000);
  }

  // If slow button is pressed, toggle boolean for slower output
  if (slowButton.getSingleDebouncedPress())
  {
    Serial.println("\nToggling slow mode...\n");
    slow = !slow;
    delay(1000);
  }  

  /* Reading both sensors and printing output.
     The 'if' statement allows us to safely continue
     execution if a hardware failure occurs. (400ms startup delay)       
  */
  if (drag.wait_ready_timeout(400) && lift.wait_ready_timeout(400))
  {
    Serial.print("Drag:\t");
    Serial.print(drag.get_units(), decimalPrecision);
    Serial.print("  ");
    Serial.print(UNITS);
    Serial.print("\t\tLift:\t");
    Serial.print(lift.get_units(), decimalPrecision);
    Serial.print("  ");
    Serial.println(UNITS);
  }
  else
  {
    // Hardware failure, sensors did not initialize in time
    Serial.println("HX711 not found. Please check connections.");
    delay(5000);
  }

  // delay print output while slow mode is on;
  // only needed for serial
  if(slow)
  {
    delay(250);
  }
}