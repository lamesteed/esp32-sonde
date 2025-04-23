#include "CTestModeCommand.h"
#include "ProbeSampler.h"
#include "CDummySampler.h"
#include "IDataPublisherService.h"
#include "CSystemTimeService.h"

#include <memory>
#include <sstream>
#include <algorithm>

CTestModeCommand::CTestModeCommand(
    const ISampler::Ptr & sampler,
    const ISampleSerializer::Ptr & serializer,
    const IStorageService::Ptr & storage,
    const IDataPublisherService::Ptr & publisher,
    int samplesCount )
        : mSampler( sampler )
        , mSerializer( serializer )
        , mStorageService( storage )
        , mPublisher( publisher )
        , mSamplesCount( samplesCount )
{
}

std::string CTestModeCommand::getDescription() const
{
    // create string that specifies command name and its parameters(how many samples to take)
    return "TESTMODE: samplesCount=" + std::to_string( mSamplesCount );
}

std::map<std::string, std::string> CTestModeCommand::readConfig( const std::string & filename ) const
{
    // retrieve configuration parameters from storage
    std::map<std::string, std::string> config;
    std::string configData;
    if ( !mStorageService->readData( filename, configData ) )
    {
        return config;
    }

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

    return config;
}


bool CTestModeCommand::execute()
{
    // retrieve sampler configuration parameters from storage
    ISampler::CalibrationConfig config = readConfig( "sampler.cfg" );
    if ( config.empty() )
    {
        mPublisher->publishData( "Failed to read sampler configuration", true );
        return false;
    }
    //retrieve metadata configuration parameters from storage
    ISampleSerializer::MetadataConfig metadata = readConfig( "metadata.cfg" );
    if ( metadata.empty() )
    {
        mPublisher->publishData( "Failed to read metadata configuration", true );
        return false;
    }

    // initialize sampler
    if ( !mSampler->init( config ) )
    {
        mPublisher->publishData( "Sampler initialization failed", true );
        return false;
    }

    // reset serializer
    mSerializer->reset( metadata );

    // store serialized sample to storage
    std::string filename = "testmode.csv";

    // collect samples mSampleCount times
    for ( int i = 0; i < mSamplesCount; i++ )
    {
        // get next sample
        SampleData::Ptr sample = mSampler->getSample();

        // serialize sample
        std::ostringstream oss;
        if ( !mSerializer->serialize( sample, oss ) )
        {
            mPublisher->publishData( "Failed to serialize sample", true );
            return false;
        }

        if (i == 0)
        {
            // For the first sample - re-create file
            if ( !mStorageService->storeData( filename, oss.str() ) )
            {
                mPublisher->publishData( "Failed to store fiile", true );
                return false;
            }
        }
        else
        {
            // append next sample to file
            if ( !mStorageService->appendData( filename, oss.str() ) )
            {
                mPublisher->publishData( "Failed to store fiile", true );
                return false;
            }
        }
    }

    // read file from storage and publish it
    std::string fileData;
    if ( !mStorageService->readData( filename, fileData ) )
    {
        mPublisher->publishData( "Failed to read file", true );
        return false;
    }

    if ( !mPublisher->publishData( fileData, true ) )
    {
        mPublisher->publishData( "Failed to publish file", true );
        return false;
    }

    return true;
}