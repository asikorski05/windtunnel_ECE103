Program and supporting circtuitry for our wind tunnel group project.

Calibrate the load sensors with the CALIBRATE_LOAD_CELLS.ino file.
Calibrate the wind sensor with the CALIBRATE_WIND_SENSOR.ino file.
Instructions for calibrating are in each file.

The WINDTUNNEL.ino file is the main script for the Arduino Uno.
Place calibration values in the labeled variables inside this file.

WINDTUNNEL.ino will display current values for drag, lift, and wind speed.
The LCD is optional; results are also printed to the serial monitor with more detail.

A seperate 5V power supply should be connected to the power rails, as the wind sensor cannot draw enough current from the arduino alone.
Without a power supply, the wind speed may not be acccurate and/or the circuit will not function.
