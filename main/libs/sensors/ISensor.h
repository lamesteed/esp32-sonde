#ifndef ISENSOR_H
#define ISENSOR_H

#include <memory>
#include <string>
#include <map>

class ISensor
{
public:
    using Ptr = std::shared_ptr<ISensor>;

    // Calibration configuration key-value pairs
    // Key is parameter name, value is parameter value in string format
    using CalibrationConfig = std::map<std::string, std::string>;

    // Expected list of calibratition parameter names and their default values in string format
    // To be used by CalibrationConfigHelper.
    // Default values are used if parameter is not present in the CalibrationConfig
    using CalibrationParams = std::map<std::string, std::string>;

    /// @brief Virtual destructor
    virtual ~ISensor() = default;

    /// @brief Initialize sampler
    virtual bool init( const CalibrationConfig & config ) = 0;

};

#endif // ISENSOR_H