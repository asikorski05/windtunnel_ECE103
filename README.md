Program and supporting circtuitry for our wind tunnel group project.

Calibrate the load sensors with the CALIBRATE_LOAD_CELLS.ino file.
Calibrate the wind sensor with the CALIBRATE_WIND_SENSOR.ino file.
Instructions for calibrating are in each file.

  Use standard 1kg load cells with HX711 amplifiers.
  The thermal wind speed sensor was sourced from here: https://moderndevice.com/products/wind-sensor

  Please note the fritzing diagram to wire each sensor.
  A seperate 5V power supply must be connected to the power rails, as the wind sensor needs more current than the arduino can provide.

The WINDTUNNEL.ino file is the main script for the Arduino Uno.
Place the calibration values into the labeled variables inside this file.

  WINDTUNNEL.ino will measure current values for drag, lift, and wind speed.
  Results are printed to the serial monitor and the LCD; the serial monitor will have more detail.


If there is a sensor error, please check that digital pins 2-5 are connected properly.
If wind speed is erratic or unrealistic, please check that a seperate 5V power supply is connected and the analog pins 0-1 are connected properly.
