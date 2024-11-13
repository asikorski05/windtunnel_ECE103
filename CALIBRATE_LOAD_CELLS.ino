// Calibrating each load cell. This is required
// due to manufacturing differences. 
// Written by Alex Sikorski for ECE103. Revision 2.
// HX711 Library used from: https://github.com/bogde/HX711/releases

#include <Arduino.h>
#include "HX711.h"
#include <Pushbutton.h>

// circuit wiring
const int DRAG_DOUT_PIN = 2;
const int DRAG_SCK_PIN = 3;
const int LIFT_DOUT_PIN = 4;
const int LIFT_SCK_PIN = 5;
const int TARE_BUTTON_PIN = 7;  // Connected to 10K pulldown resistor

// Setting up load sensor library
  HX711 drag;
  HX711 lift;

// Initilize Button
  Pushbutton tareButton(TARE_BUTTON_PIN);

void setup() {

  // Start Serial Monitor with 57600 baud rate
  Serial.begin(57600);

  Serial.println("Setting up drag sensor. (parallel to tunnel)");
  drag.begin(DRAG_DOUT_PIN, DRAG_SCK_PIN);
  Serial.println("Setting up lift sensor. (perpendicular to tunnel)");
  lift.begin(LIFT_DOUT_PIN, LIFT_SCK_PIN);
  Serial.print("Setup Complete! \n \n");
  delay(1000);
}

void loop() {

  if (drag.is_ready())
  {
    drag.set_scale();   // No calibration factor yet, no arg
    Serial.println("Remove any weights from the drag sensor and press the tare button.");
    tareButton.waitForButton();
    drag.tare();
    Serial.println("Tare done.");
    Serial.println("Place a known weight on the drag sensor and press the tare button.");
    tareButton.waitForButton();
    long reading = drag.get_units(10);
    Serial.print("Drag Result: ");
    Serial.println(reading);
    Serial.println("Calibration factor is the (result)/(known weight) preferably in grams.");
    Serial.println("Please compute the factor and insert it into the main script.");
    Serial.println("Press the tare button to continue...");
    tareButton.waitForButton();
  } 
  else 
  {
    // Hardware failure, sensors did not initialize
    Serial.println("Drag sensor not found. Please check connections.");
    delay(5000);
  }

  // Repeat block for lift
  if (lift.is_ready())
  {
    lift.set_scale();    
    Serial.println("Remove any weights from the lift sensor and press the tare button.");
    tareButton.waitForButton();
    lift.tare();
    Serial.println("Tare done.");
    Serial.println("Place a known weight on the lift sensor and press the tare button.");
    tareButton.waitForButton();
    long reading = lift.get_units(10);
    Serial.print("Lift Result: ");
    Serial.println(reading);
    Serial.println("Calibration factor is the (result)/(known weight) preferably in grams.");
    Serial.println("Please compute the factor and insert it into the main script.");
    Serial.println("Press the tare button to continue...");
    tareButton.waitForButton();
  } 
  else 
  {
    Serial.println("Lift sensor not found. Please check connections.");
    delay(5000);
  }
}
