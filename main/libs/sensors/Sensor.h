#ifndef SENSOR_H
#define SENSOR_H

#include "ISensor.h"

class OneWire;
class DallasTemperature;

class Sensor : public ISensor
{
public:
    /// @brief Constructor
    Sensor();

    /// @brief Virtual destructor
    virtual ~Sensor();

    float getAnalogInputVoltage (std::string inputPin); // For sensors connected through the ADC1115 
    float getAnalogInputVoltage (int inputPin);         // For sensors connected directly to analog input
    float getValue (float input_voltage, float factorA, float factorB); // Based on voltage and calibration values

private:
    // ISampler interface

    /// @brief Initialize sampler, must be called before first call to getSample()
    ///        After this call sampler considered ready to provide samples
    /// @return true if initialization successful, false otherwise
    virtual bool init() override;

private:

    static const char * TAG;

};

#endif // SENSOR_H