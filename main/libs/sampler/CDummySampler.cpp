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

SampleData::Ptr CDummySampler::getSample()
{
    // Create a new sample data object
    SampleData::Ptr sample = std::make_shared<SampleData>();
    // Fill the sample data with dummy values
    sample->temperature = 25.0f; // Dummy temperature
    sample->pressure = 1013.25f; // Dummy pressure
    sample->tds = 500.0f;        // Dummy TDS
    sample->conductivity = 1000.0f; // Dummy conductivity
    sample->ph = 7.0f;          // Dummy pH
    sample->do2 = 8.0f;         // Dummy DO2
    sample->depth = 0.0f;       // Dummy depth
    sample->tsStart = 0;        // Dummy start timestamp
    sample->tsEnd = 0;          // Dummy end timestamp
    // Return the sample data
    return sample;
}
