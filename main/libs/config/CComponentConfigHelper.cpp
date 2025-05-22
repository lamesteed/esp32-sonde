#include "CComponentConfigHelper.h"
#include <algorithm>
#include <climits>
#include <math.h>

CComponentConfigHelper::CComponentConfigHelper (
    const ComponentConfig & config,
    const ComponentConfigDefaults & expectedParameters )
        : mConfig( config )
        , mParams( expectedParameters )
{
}

std::string CComponentConfigHelper::getAsString( const std::string & key) const
{
    // Find paramenter in the configuration, if found return its value
    auto it = mConfig.find( key );
    if ( it != mConfig.end() )
    {
        return it->second;
    }

    // If parameter not found, try to find it in the expected parameters
    auto it2 = mParams.find( key );
    if ( it2 != mParams.end() )
    {
        return it2->second;
    }

    // If parameter not found in the expected parameters, return empty string
    return "";
}

int CComponentConfigHelper::getAsInt( const std::string & key) const
{
    // Get parameter value as string
    std::string value = getAsString( key );

    // Convert string to integer
    return ( value.empty() ) ? INT_MIN : std::stoi( value );
}

float CComponentConfigHelper::getAsFloat( const std::string & key) const
{
    // Get parameter value as string
    std::string value = getAsString( key );

    // Convert string to float
    return ( value.empty() ) ? NAN : std::stof( value );
}

bool CComponentConfigHelper::getAsBool( const std::string & key) const
{
    // Get parameter value as string
    std::string value = getAsString( key );

    // Convert string to boolean using following rules:
    // "true" case insensitive -> true
    // "false" case insensitive -> false
    // "0" -> false
    // "1" -> true
    // any other value -> false

    // convert value to lower case
    std::transform( value.begin(), value.end(), value.begin(), ::tolower );

    return ( value.empty() ) ? false : ( value == "true" || value == "1" );
}