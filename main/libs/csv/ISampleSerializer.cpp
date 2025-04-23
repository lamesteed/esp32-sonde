#include "ISampleSerializer.h"

#include <iomanip>
#include <sstream>

std::string ISampleSerializer::toTwoDecimalString( float value )
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    return oss.str();
}
