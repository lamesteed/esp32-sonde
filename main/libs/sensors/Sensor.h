#ifndef SENSORS_SENSOR_H
#define SENSORS_SENSOR_H

#include <iomanip>
#include <sstream>
#include "CCalibrationConfigHelper.h"

class Sensor {
private:
float calculateFromVoltage(float input_voltage, const std::string &factorAKey, const std::string &factorBKey);
    // Constants for voltage reference and ADC resolution
    static constexpr int REF_VOLTAGE = 5; // Reference voltage in volts
    static constexpr int ADC_COMPENSATION = 1; // Compensation factor for ADC
    static constexpr int ADC_RESOLUTION = 4096; // ADC resolution (12-bit)
    static const char * TAG;

    std::string mFactorAKey; // Calibration key for factor A
    std::string mFactorBKey; // Calibration key for factor B
    CCalibrationConfigHelper::Ptr mConfigHelper;        ///< Calibration configuration helper

public:

    Sensor(const std::string &factorAKey, const std::string &factorBKey); // Constructor
    float getAnalogInputVoltage(int inputPin);
    float getSensorReading(float input_voltage);
};

#endif  // SENSORS_SENSOR_H