#include "CDummySampler.h"

CDummySampler::CDummySampler( const ITimeService::Ptr & timeService )
    : mTimeService( timeService )
    , mSamples(120)
    , mSampleIndex( 0 )
{
    // Initialize the sample data with dummy values
    for (int i = 0; i < 60; ++i) {
        mSamples[i] = std::make_shared<SampleData>();
        mSamples[i]->temperature = 25.0f + i; // Dummy temperature
        mSamples[i]->tds = 500.0f + i;        // Dummy TDS
        mSamples[i]->conductivity = 1000.0f + i; // Dummy conductivity
        mSamples[i]->ph = 7.0f + i;          // Dummy pH
        mSamples[i]->do2 = 8.0f + i;         // Dummy DO2
        mSamples[i]->depth = 0.0f + i;       // Dummy depth
        mSamples[i]->tsStart = 0;            // Dummy start timestamp
        mSamples[i]->tsEnd = 0;              // Dummy end timestamp
        mSamples[i]->depth = -1 * (0.98 * (i + 1) ); // Dummy depth
    }

    for (int i = 60; i < 120; ++i) {
        mSamples[i] = std::make_shared<SampleData>();
        mSamples[i]->temperature = 25.0f + i; // Dummy temperature
        mSamples[i]->tds = 500.0f + i;        // Dummy TDS
        mSamples[i]->conductivity = 1000.0f + i; // Dummy conductivity
        mSamples[i]->ph = 7.0f + i;          // Dummy pH
        mSamples[i]->do2 = 8.0f + i;         // Dummy DO2
        mSamples[i]->depth = 0.0f + i;       // Dummy depth
        mSamples[i]->tsStart = 0;            // Dummy start timestamp
        mSamples[i]->tsEnd = 0;              // Dummy end timestamp
        mSamples[i]->depth = -1 * (120 - 0.99 * i);  // Dummy depth
    }

    mSamples[119]->depth = 0.0f; // Dummy depth for the last sample

    // calculate pressure using formula:  pressure, psi  = depth, m / -0.703
    for (int i = 0; i < 120; ++i) {
        mSamples[i]->pressure = mSamples[i]->depth / -0.703;
    }

}

CDummySampler::~CDummySampler()
{

}

bool CDummySampler::init( const CalibrationConfig & config )
{
    mSampleIndex = 0;

    return true;

}

SampleData::Ptr CDummySampler::getSample()
{
    int index = mSampleIndex % mSamples.size();

    mSampleIndex++;
    mSamples[index]->tsStart = mTimeService->getTime();
    mSamples[index]->tsEnd = mTimeService->getTime() + 2000; // 2 seconds later
    return mSamples[index];
}
