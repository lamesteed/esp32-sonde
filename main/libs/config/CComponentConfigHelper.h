#ifndef CCOMPONENTCONFIGHELPER_H
#define CCOMPONENTCONFIGHELPER_H

#include "ComponentConfig.h"

#include <memory>

/// @brief Helper class to retrieve and format configuration paramenters or return default values if actual parameters are not present
class CComponentConfigHelper
{
    public:
    using Ptr = std::shared_ptr<CComponentConfigHelper>;

        /// @brief Constructor
        /// @param config Component configuration (key - parameter name, value - parameter value in string format)
        /// @param expectedParameters Expected list of configurable parameters with their default values
        CComponentConfigHelper (
            const ComponentConfig & config,
            const ComponentConfigDefaults & expectedParameters );

        /// @brief Virtual destructor
        virtual ~CComponentConfigHelper() = default;

    /// @brief Retrieve config parameter value as string
    /// @param Parameter name
    /// @return parameter value as string or empty string if parameter not found
    std::string getAsString( const std::string & key) const;

    /// @brief Retrieve config parameter value as integer
    /// @param Parameter name
    /// @return parameter value as integer or INT_MIN if parameter not found
    int getAsInt( const std::string & key) const;

    /// @brief Retrieve config parameter value as float
    /// @param Parameter name
    /// @return parameter value as float or NAN if parameter not found
    float getAsFloat( const std::string & key) const;

    /// @brief Retrieve config parameter value as boolean
    /// @param Parameter name
    /// @return parameter value as boolean or false if parameter not found
    bool getAsBool( const std::string & key) const;

    private:
    ComponentConfig mConfig;            ///< List of configuration paramters to apply
    ComponentConfigDefaults mParams;    ///< List of expected configurable parameters with their default values
};


#endif // CCOMPONENTCONFIGHELPER_H