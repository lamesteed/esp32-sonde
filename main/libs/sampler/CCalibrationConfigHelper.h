#ifndef CCALIBRATIONCONFIGHELPER_H
#define CCALIBRATIONCONFIGHELPER_H

#include "ISampler.h"

class CCalibrationConfigHelper
{
    public:
    using Ptr = std::shared_ptr<CCalibrationConfigHelper>;

        /// @brief Constructor
        /// @param config Calibration configuration (key - parameter name, value - parameter value in string format)
        /// @param expectedParameters Expected list of calibration parameters with their default values
        CCalibrationConfigHelper (
            const ISampler::CalibrationConfig & config,
            const ISampler::CalibrationParams & expectedParameters );

        /// @brief Virtual destructor
        virtual ~CCalibrationConfigHelper() = default;

    /// @brief Retrieve calibration parameter value as string
    /// @param Parameter name
    /// @return parameter value as string or empty string if parameter not found
    std::string getAsString( const std::string & key) const;

    /// @brief Retrieve calibration parameter value as integer
    /// @param Parameter name
    /// @return parameter value as integer or INT_MIN if parameter not found
    int getAsInt( const std::string & key) const;

    /// @brief Retrieve calibration parameter value as float
    /// @param Parameter name
    /// @return parameter value as float or NAN if parameter not found
    float getAsFloat( const std::string & key) const;

    /// @brief Retrieve calibration parameter value as boolean
    /// @param Parameter name
    /// @return parameter value as boolean or false if parameter not found
    bool getAsBool( const std::string & key) const;

    private:
    ISampler::CalibrationConfig mConfig;    ///< Calibration configuration
    ISampler::CalibrationParams mParams;    ///< Expected list of expected calibration parameters with their default values
};


#endif // CCALIBRATIONCONFIGHELPER_H