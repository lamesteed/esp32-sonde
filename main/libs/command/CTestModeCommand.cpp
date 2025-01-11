#include "CTestModeCommand.h"
#include "ProbeSampler.h"
#include "CDummySampler.h"
#include "IDataPublisherService.h"

#include <memory>

CTestModeCommand::CTestModeCommand(
    const ISampler::Ptr & sampler,
    const IDataPublisherService::Ptr & publisher, int samplesCount )
        : mSampler( sampler )
        , mPublisher( publisher )
        , mSamplesCount( samplesCount )
{

}

std::string CTestModeCommand::getDescription() const
{
    // create string that specifies command name and its parameters(how many samples to take)
    return "TESTMODE: samplesCount=" + std::to_string( mSamplesCount );
}

bool CTestModeCommand::execute()
{
    // initialize sampler
    if ( !mSampler->init() )
    {
        mPublisher->publishData( "Sampler initialization failed", true );
        return false;
    }

    // collect samples mSampleCount times and publish each sample
    for ( int i = 0; i < mSamplesCount; i++ )
    {
        std::string sample = mSampler->getSample();
        if ( sample.empty() )
        {
            mPublisher->publishData( "Failed to get next sample", true );
            return false;
        }

        bool sendEOD = ( i == mSamplesCount - 1 );

        if ( !mPublisher->publishData( sample, sendEOD ) )
        {
            return false;
        }
    }
    return true;
}