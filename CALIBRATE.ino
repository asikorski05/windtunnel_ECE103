// Calibrating each load cell. This is required
// due to manufacturing differences. 
// We will use https://github.com/bogde/HX711/releases/
// In the serial monitor, use a baud rate of 57600 to see an output.
#include "HX711.h"

// circuit wiring
const int DRAG_DOUT_PIN = 2;
const int DRAG_SCK_PIN = 3;     // PWM
const int LIFT_DOUT_PIN = 4;
const int LIFT_SCK_PIN = 5;     // PWM
#define RV_ANALOG_PIN = 1;      // Analog
#define TMP_ANALOG_PIN = 0;     // Analog

// to calibrate wind sensor, put a glass over it, but the sensor should not be
// touching any surface.
// adjust the zeroWindAdjustment until your sensor reads about zero with the glass over it. 

const float zeroWindAdjustment =  0.2; // negative numbers yield smaller wind speeds and vice versa.

// WIND SPEED VARIABLES FOR CALCULATION
  int TMP_Therm_ADunits;  //temp termistor value from wind sensor
  float RV_Wind_ADunits;    //RV output from wind sensor 
  float RV_Wind_Volts;
  unsigned long lastMillis;
  int TempCtimes100;
  float zeroWind_ADunits;
  float zeroWind_volts;
  float WindSpeed_MPH;

// Setting up load sensor library
HX711 drag;
HX711 lift;

void setup() {
  // Start Serial Monitor with 57600 baud rate
  Serial.begin(57600);

  Serial.println("Setting up drag sensor. (parallel to tunnel)")
  drag.begin(DRAG_DOUT_PIN, DRAG_SCK_PIN);
  Serial.println("Setting up lift sensor. (perpendicular to tunnel)")
  lift.begin(LIFT_DOUT_PIN, LIFT_SCK_PIN);

  // Resetting analog pins for thermistor
  pinMode(A2, INPUT);        // GND pin      
  pinMode(A3, INPUT);        // VCC pin
  digitalWrite(A3, LOW);     // turn off pullups
}

void loop() {

  if (drag.is_ready())
  {
    drag.set_scale();    
    Serial.println("Tare: remove any weights from the drag scale.");
    delay(5000);    // wait 5 seconds
    drag.tare();
    Serial.println("Tare done.");
    Serial.print("Place a known weight on the drag scale.");
    delay(5000);
    long reading = drag.get_units(10);
    Serial.print("Drag Result: ");
    Serial.println(reading);
    Serial.println("Calibration factor will be the reading/known weight.")
    Serial.println("Please save and insert this factor into the main script.")
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
    Serial.println("Tare: remove any weights from the lift scale.");
    delay(5000);    // wait 5 seconds
    lift.tare();
    Serial.println("Tare done.");
    Serial.print("Place a known weight on the lift scale.");
    delay(5000);
    long reading = lift.get_units(10);
    Serial.print("Lift Result: ");
    Serial.println(reading);
    Serial.println("Calibration factor will be the reading/known weight.")
    Serial.println("Please save and insert this factor into the main script.")
  }
  else 
  {
    Serial.println("Lift sensor not found. Please check connections.");
    delay(5000);
  }

  // Wind sensor readings for manual calibration. Code used from repository.
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
   
    Serial.print("  TMP volts: ");
    Serial.print(TMP_Therm_ADunits * 0.0048828125);
    
    Serial.print(" RV volts: ");
    Serial.print((float)RV_Wind_Volts);

    Serial.print("\t  TempC*100 ");
    Serial.print(TempCtimes100 );

    Serial.print("   ZeroWind volts: ");
    Serial.print(zeroWind_volts);

    Serial.print("   WindSpeed MPH: ");
    Serial.println((float)WindSpeed_MPH);
    Serial.println("Wind speed should read approx zero when properly calibrated!")
    lastMillis = millis();
    delay(5000);    // allow user to read data
  }
}