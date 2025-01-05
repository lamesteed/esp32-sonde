#include "CTestModeCommand.h"
#include "ProbeSampler.h"
#include "CDummySampler.h"
#include "IDataPublisherService.h"

#include <memory>

CTestModeCommand::CTestModeCommand( IDataPublisherService & publisher, int samplesCount )
    : mPublisher( publisher )
    , mSamplesCount( samplesCount )
{

}

CTestModeCommand::~CTestModeCommand()
{

}

std::string CTestModeCommand::getDescription() const
{
    // create string that specifies command name and its parameters(how many samples to take)
    return "TESTMODE: samplesCount=" + std::to_string( mSamplesCount );
}

bool CTestModeCommand::execute()
{
    // Create and init sampler
    std::shared_ptr<ISampler> sampler( new ProbeSampler( mSamplesCount ) );
//    std::shared_ptr<ISampler> sampler( new CDummySampler() );
    if ( !sampler->init() )
    {
        mPublisher.publishData( "Sampler initialization failed", true );
        return false;
    }

    // collect samples mSampleCount times and publish each sample
    for ( int i = 0; i < mSamplesCount; i++ )
    {
        std::string sample = sampler->getSample();
        if ( sample.empty() )
        {
            mPublisher.publishData( "Failed to get next sample", true );
            return false;
        }

        bool sendEOD = ( i == mSamplesCount - 1 );

        if ( !mPublisher.publishData( sample, sendEOD ) )
        {
            return false;
        }
    }
    return true;
}