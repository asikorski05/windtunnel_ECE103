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
  


  /*
  Serial.println("Now calibrating wind speed sensor: ");

  // Wind sensor readings for manual calibration. Code modified from repository.
  if (millis() - lastMillis > 200)      // read every 200 ms - printing slows this down further
  {
    TMP_Therm_ADunits = analogRead(TMP_ANALOG_PIN);
    RV_Wind_ADunits = analogRead(RV_ANALOG_PIN);
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
   
    Serial.print("  TMP volts: ");
    Serial.print(TMP_Therm_ADunits * 0.0048828125);
    
    Serial.print(" RV volts: ");
    Serial.print((float)RV_Wind_Volts);

    Serial.print("\t  TempC*100 ");
    Serial.print(TempCtimes100 );

    Serial.print("   ZeroWind volts: ");
    Serial.print(zeroWind_volts);

    Serial.print("\n   WindSpeed MPH: ");
    Serial.println((float)WindSpeed_MPH);
    Serial.println("Wind speed should read approx zero when properly calibrated!");
    lastMillis = millis();
    Serial.println("Press the tare button to continue...");
    tareButton.waitForButton();
  }
      */
}