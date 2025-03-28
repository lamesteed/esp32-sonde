#include "CDummySampler.h"

CDummySampler::CDummySampler()
{

}

CDummySampler::~CDummySampler()
{

}

bool CDummySampler::init( const CalibrationConfig & config )
{
    return true;

}

std::string CDummySampler::getSample()
{
    static int counter = 0;
    counter++;

    return "Temperature: " +
           std::to_string( counter ) + " °C, Pressure: " +
           std::to_string( counter ) + " psi, TDS: " +
           std::to_string( counter ) + " ppm, Conductivity: " +
           std::to_string( counter ) + " μS/cm\n";
}
