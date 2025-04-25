#include "CDummySampler.h"

CDummySampler::CDummySampler( const ITimeService::Ptr & timeService )
    : mTimeService( timeService )
    , mSamples(10)
    , mSampleIndex( 0 )
{
    // Initialize the sample data with dummy values
    for (int i = 0; i < 10; ++i) {
        mSamples[i] = std::make_shared<SampleData>();
        mSamples[i]->temperature = 25.0f + i; // Dummy temperature
        mSamples[i]->pressure = 1013.25f + i; // Dummy pressure
        mSamples[i]->tds = 500.0f + i;        // Dummy TDS
        mSamples[i]->conductivity = 1000.0f + i; // Dummy conductivity
        mSamples[i]->ph = 7.0f + i;          // Dummy pH
        mSamples[i]->do2 = 8.0f + i;         // Dummy DO2
        mSamples[i]->depth = 0.0f + i;       // Dummy depth
        mSamples[i]->tsStart = 0;            // Dummy start timestamp
        mSamples[i]->tsEnd = 0;              // Dummy end timestamp
    }

    mSamples[0]->depth = -0.9;
    mSamples[1]->depth = -2.05;
    mSamples[2]->depth = -2.98;
    mSamples[3]->depth = -4.1;
    mSamples[4]->depth = -5.05; // bottom
    mSamples[5]->depth = -3.99;
    mSamples[6]->depth = -3.07;
    mSamples[7]->depth = -1.92;
    mSamples[8]->depth = -1.1;
    mSamples[9]->depth = 0;

    // calculate pressure using formula:  pressure, psi  = depth, m / -0.703
    for (int i = 0; i < 10; ++i) {
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
