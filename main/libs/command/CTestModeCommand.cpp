#include "CTestModeCommand.h"
#include "ProbeSampler.h"
#include "CDummySampler.h"
#include "IDataPublisherService.h"

#include <memory>
#include <sstream>
#include <algorithm>

CTestModeCommand::CTestModeCommand(
    const ISampler::Ptr & sampler,
    const IDataPublisherService::Ptr & publisher,
    const IStorageService::Ptr & storage,
    int samplesCount )
        : mSampler( sampler )
        , mPublisher( publisher )
        , mStorageService( storage )
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
    // retrieve configuration parameters from storage
    std::string configData;
    if ( !mStorageService->readData( "sampler.cfg", configData ) )
    {
        mPublisher->publishData( "Failed to read configuration data", true );
        return false;
    }
    ISampler::CalibrationConfig config;

    // parse configuration parameters stored in key=value format, drop any spaces in key or value
    std::istringstream iss( configData );
    std::string line;
    while ( std::getline( iss, line )
            && line.find( '=' ) != std::string::npos )
    {
        // split line into key and value
        std::string key = line.substr( 0, line.find( '=' ) );
        std::string value = line.substr( line.find( '=' ) + 1 );
        // remove spaces from key and value
        key.erase( std::remove_if( key.begin(), key.end(), ::isspace ), key.end() );
        value.erase( std::remove_if( key.begin(), key.end(), ::isspace ), key.end() );

        // add key and value to configuration
        config[key] = value;
    }

    // initialize sampler
    if ( !mSampler->init( config ) )
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