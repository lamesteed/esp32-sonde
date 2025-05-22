#ifndef COMPONENTCONFIG_H
#define COMPONENTCONFIG_H

#include <string>
#include <map>


// Actual configuration parameters for configurable components
// Key is parameter name, value is parameter value in string format
using ComponentConfig = std::map<std::string, std::string>;

// Expected list of configuration parameter names and their default values in string format
// Used to validate and adjust actual configuration parameters
// Default values are used if parameter is not present in the CalibrationConfig
using ComponentConfigDefaults = std::map<std::string, std::string>;

#endif // COMPONENTCONFIG_H