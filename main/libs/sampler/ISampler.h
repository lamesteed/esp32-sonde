#ifndef ISAMPLER_H
#define ISAMPLER_H

#include <memory>
#include <string>
#include <map>
#include "SampleData.h"

class ISampler
{
public:
    using Ptr = std::shared_ptr<ISampler>;

    // Calibration configuration key-value pairs
    // Key is parameter name, value is parameter value in string format
    using CalibrationConfig = std::map<std::string, std::string>;

    // Expected list of calibratition parameter names and their default values in string format
    // To be used by CalibrationConfigHelper.
    // Default values are used if parameter is not present in the CalibrationConfig
    using CalibrationParams = std::map<std::string, std::string>;

    /// @brief Virtual destructor
    virtual ~ISampler() = default;

    /// @brief Initialize sampler
    virtual bool init( const CalibrationConfig & config ) = 0;

    /// @brief  Retrieve next sample from sensors
    /// @return Sample data
    virtual  SampleData::Ptr getSample() = 0;
};

#endif // ISAMPLER_H