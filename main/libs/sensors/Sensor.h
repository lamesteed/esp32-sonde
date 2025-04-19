#ifndef SENSOR_H
#define SENSOR_H

#include "ISensor.h"
#include "IStorageService.h"

class OneWire;
class DallasTemperature;

class Sensor : public ISensor
{
public:
    /// @brief Constructor
    Sensor();

    /// @brief Virtual destructor
    virtual ~Sensor();

    float getAnalogInputVoltage (int inputPin);
    float getValue (float input_voltage, float factorA, float factorB); // Updated method

private:
    // ISampler interface

    /// @brief Initialize sampler, must be called before first call to getSample()
    ///        After this call sampler considered ready to provide samples
    /// @return true if initialization successful, false otherwise
    virtual bool init( const CalibrationConfig & config ) override;

private:

    static const char * TAG;

};

#endif // SENSOR_H