## Calibration

Preparing standard solutions for low and high conductivity at home can be done using common household items and distilled water. Here are some steps to prepare these solutions:

Low Conductivity Solution
A low conductivity solution can be made using distilled water and a small amount of table salt (sodium chloride).

Materials Needed:

Distilled water
Table salt (sodium chloride)
A precise scale
A clean container
A stirrer
Procedure:

Measure 1 liter of distilled water and pour it into the clean container.
Weigh 0.74 grams of table salt.
Dissolve the salt in the distilled water by stirring until completely dissolved.
This solution will have a conductivity of approximately 1413 µS/cm.
High Conductivity Solution
A high conductivity solution can be made using distilled water and a larger amount of table salt.

Materials Needed:

Distilled water
Table salt (sodium chloride)
A precise scale
A clean container
A stirrer
Procedure:

Measure 1 liter of distilled water and pour it into the clean container.
Weigh 7.45 grams of table salt.
Dissolve the salt in the distilled water by stirring until completely dissolved.
This solution will have a conductivity of approximately 12,880 µS/cm.
Calibration Steps
Low Point Calibration:

Immerse the EC sensor in the low conductivity solution.
Measure the voltage and temperature.
Call the calibration method with the appropriate command (e.g., ec.calibration(voltage, temperature, "CAL,LOW")).
High Point Calibration:

Immerse the EC sensor in the high conductivity solution.
Measure the voltage and temperature.
Call the calibration method with the appropriate command (e.g., ec.calibration(voltage, temperature, "CAL,HIGH")).
Example Code
Here is an example of how to perform the calibration in your code:


#include <DFRobot_ESP_EC.h>

DFRobot_ESP_EC ec;

void setup() {
  Serial.begin(9600);
  ec.begin();
}

void loop() {
  float voltage = readVoltageFromSensor(); // Replace with actual voltage reading code
  float temperature = readTemperature();   // Replace with actual temperature reading code

  // Perform low point calibration
  ec.calibration(voltage, temperature, "CAL,LOW");

  // Perform high point calibration
  ec.calibration(voltage, temperature, "CAL,HIGH");

  delay(1000);
}

float readVoltageFromSensor() {
  // Implement voltage reading from sensor
  return 1.23; // Example voltage
}

float readTemperature() {
  // Implement temperature reading
  return 25.0; // Example temperature
}

By following these steps, you can prepare low and high conductivity solutions at home and use them to calibrate your EC sensor accurately.

The result of the calibration is typically stored in non-volatile memory (such as EEPROM) so that the calibration data is retained even when the device is powered off. This allows the sensor to use the calibrated values for accurate measurements without needing to recalibrate every time the device is restarted.

In the DFRobot_ESP_EC library, the calibration data is stored in EEPROM. Here is a brief overview of how it works:

Calibration Data Storage: The calibration method adjusts the internal parameters (_kvalueLow, _kvalueHigh, etc.) based on the calibration process.
EEPROM Storage: These parameters are then stored in EEPROM so that they can be retrieved and used in future measurements.
Here is an example of how the calibration data might be stored and retrieved in the DFRobot_ESP_EC library:

#include <EEPROM.h>
#include <DFRobot_ESP_EC.h>

DFRobot_ESP_EC ec;

void setup() {
  Serial.begin(9600);
  ec.begin();
  // Load calibration data from EEPROM
  ec.loadCalibrationData();
}

void loop() {
  float voltage = readVoltageFromSensor(); // Replace with actual voltage reading code
  float temperature = readTemperature();   // Replace with actual temperature reading code

  // Perform low point calibration
  ec.calibration(voltage, temperature, "CAL,LOW");

  // Perform high point calibration
  ec.calibration(voltage, temperature, "CAL,HIGH");

  // Save calibration data to EEPROM
  ec.saveCalibrationData();

  delay(1000);
}

float readVoltageFromSensor() {
  // Implement voltage reading from sensor
  return 1.23; // Example voltage
}

float readTemperature() {
  // Implement temperature reading
  return 25.0; // Example temperature
}

In this example:

ec.loadCalibrationData() loads the calibration data from EEPROM during setup.
ec.saveCalibrationData() saves the calibration data to EEPROM after performing the calibration.
This ensures that the calibration data is retained across power cycles and can be used for accurate measurements.


The calibration data can be saved on the ESP32 using its non-volatile storage options, such as EEPROM or the Preferences library. This ensures that the calibration data is retained even after the ESP32 is powered off.

Here is an example of how to save and load calibration data using the Preferences library on the ESP32:

##Saving Calibration Data

#include <Preferences.h>
#include <DFRobot_ESP_EC.h>

DFRobot_ESP_EC ec;
Preferences preferences;

void setup() {
  Serial.begin(9600);
  ec.begin();
  preferences.begin("ec_calib", false); // Open Preferences with namespace "ec_calib"
}

void loop() {
  float voltage = readVoltageFromSensor(); // Replace with actual voltage reading code
  float temperature = readTemperature();   // Replace with actual temperature reading code

  // Perform low point calibration
  ec.calibration(voltage, temperature, "CAL,LOW");

  // Perform high point calibration
  ec.calibration(voltage, temperature, "CAL,HIGH");

  // Save calibration data to Preferences
  preferences.putFloat("kvalueLow", ec.getKvalueLow());
  preferences.putFloat("kvalueHigh", ec.getKvalueHigh());

  delay(1000);
}

float readVoltageFromSensor() {
  // Implement voltage reading from sensor
  return 1.23; // Example voltage
}

float readTemperature() {
  // Implement temperature reading
  return 25.0; // Example temperature
}

Loading Calibration Data

#include <Preferences.h>
#include <DFRobot_ESP_EC.h>

DFRobot_ESP_EC ec;
Preferences preferences;

void setup() {
  Serial.begin(9600);
  ec.begin();
  preferences.begin("ec_calib", true); // Open Preferences with namespace "ec_calib"

  // Load calibration data from Preferences
  float kvalueLow = preferences.getFloat("kvalueLow", 1.0); // Default to 1.0 if not found
  float kvalueHigh = preferences.getFloat("kvalueHigh", 1.0); // Default to 1.0 if not found

  ec.setKvalueLow(kvalueLow);
  ec.setKvalueHigh(kvalueHigh);
}

void loop() {
  float voltage = readVoltageFromSensor(); // Replace with actual voltage reading code
  float temperature = readTemperature();   // Replace with actual temperature reading code

  // Use the EC sensor with the loaded calibration data
  float ecValue = ec.readEC(voltage, temperature);


  Serial.print("EC Value: ");
  Serial.println(ecValue);

  delay(1000);
}

float readVoltageFromSensor() {
  // Implement voltage reading from sensor
  return 1.23; // Example voltage
}

float readTemperature() {
  // Implement temperature reading
  return 25.0; // Example temperature
}

In this example:

Saving Calibration Data: The preferences.putFloat method is used to save the calibration data (kvalueLow and kvalueHigh) to non-volatile storage.
Loading Calibration Data: The preferences.getFloat method is used to load the calibration data from non-volatile storage during setup.
This ensures that the calibration data is retained across power cycles and can be used for accurate measurements.