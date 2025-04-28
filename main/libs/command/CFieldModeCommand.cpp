#include "CFieldModeCommand.h"
#include "delay.h"
#include "esp_log.h"

#include <sstream>
#include <algorithm>
#include <cmath>

const char * CFieldModeCommand::ARG_FILENAME = "filename"; ///< Name of the argument that specifies file name
const char * CFieldModeCommand::TAG = "CFieldModeCommand"; ///< Tag for logging

CFieldModeCommand::CFieldModeCommand(
    const ISampler::Ptr & sampler,
    const ISampleSerializer::Ptr & serializer,
    const IStorageService::Ptr & storage,
    const IDataPublisherService::Ptr & publisher,
    const IWatchdog::Ptr & timeService,
    const ICommand::CommandArgs & args )
    : mSampler( sampler )
    , mSerializer( serializer )
    , mStorageService( storage )
    , mPublisher( publisher )
    , mWatchdog( timeService )
    , mArgs( args )
{
}

std::string CFieldModeCommand::getDescription() const
{
    // create string that specifies command name and its parameters(how many samples to take)
    return "FIELDMODE: filename=" + mArgs.at( ARG_FILENAME );
}

bool CFieldModeCommand::execute()
{
    // retrieve ("filename"="value") from mArgs
    auto it = mArgs.find( ARG_FILENAME );
    if ( it == mArgs.end() )
    {
        std::string message = "Missing " + std::string( ARG_FILENAME ) + " argument";
        mPublisher->publishData( message, true );
        return false;
    }

    // retrieve "filename.csv" from mArgs
    std::string filename = it->second;
    std::string filenameTmp = filename;
    filenameTmp.replace( filenameTmp.find( ".csv" ), 4, "_tmp.csv" );

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

    //fetch sampling depth interval from metadata map (i.e. sample every 1 meter of depth) and convert to int
    float depthInterval =  std::stof( metadata["SAMPLING_STEP"] );
    int samplingIntervalMsec = std::stoi( metadata["SAMPLING_INTERVAL_MSEC"] );
    int maxDurationSec = std::stoi( metadata["SAMPLING_MAX_DURATION_SEC"] );
    mWatchdog->start( maxDurationSec );

    // initialize sampler
    if ( !mSampler->init( config ) )
    {
        mPublisher->publishData( "Sampler initialization failed", true );
        return false;
    }

    // reset serializer (first to be used for temporary file)
    mSerializer->reset( metadata );

    // We are ready to do sampling, pubslish message
    mPublisher->publishData( "Sampling started", true );

    // Turn off publisshing service (sonde will go under water, no wireless connection)
    mPublisher->stop();

    // Keep sampling data until surface is reached:
    // - Perform sampling every samplingIntervalMsec seconds
    // - Each sample is serialized and stored in the temporary file, called "filename_tmp.csv" (just for the reference)
    // - Also accumulate samples that math depthInterval +/- 10%  in map< float, List<SampleData>>
    // - Sampling stops, when surface is reached (based on measured pressure/depth and we have at least 1 sample in the map)
    // - Next we iterate through the map, average the samples and store them in the final file
    // - Finally, we enable publishing service so client can download the resultant file

    SamplingResultsByDepth results;
    bool appendToFile = false;
    while( true )
    {
        // sleep for X seconds
        delayMsec( samplingIntervalMsec );

        // get next sample
        SampleData::Ptr sample = mSampler->getSample();

        // store next sample to temporary file
        storeSample( sample, filenameTmp, appendToFile );

        addSampleToDepthBucket( results, sample, depthInterval );

        // check if surface is reached
        if ( ( sample->pressure < 0.1 && results.size() > 0 ) || mWatchdog->isExpired() )
        {
            // surface is reached, break the loop
            ESP_LOGI( TAG, "Surface reached, sampling stopped: pressure: %.2f psi, calculated depth: %.2f, watchdog expired: %s",
                sample->pressure,
                sample->depth,
                mWatchdog->isExpired() ? "true" : "false" );
            break;
        }

        appendToFile = true;
    }

    // Prepare final results file
    SampleDataListPtr averagedSamples = buildAverageSamplingByDepth( results );
    if ( averagedSamples )
    {
        ESP_LOGI( TAG, "Averaging samples, %d samples in the final result list", averagedSamples->size() );
        // Reset serializer (now to be used for final file)
        mSerializer->reset( metadata );
        storeSamples( averagedSamples, filename );
    }

    // Turn on publishing service
    mPublisher->start();

    return true;
}

void CFieldModeCommand::addSampleToDepthBucket( SamplingResultsByDepth & depthBuckets, const SampleData::Ptr & sample, float depthInterval ) const
{
    // Calculate the nearest bucket for this depth using rounding
    float rawBucket = sample->depth / depthInterval;
    float depthBucket = std::round(rawBucket) * depthInterval;

    // Skip surface samples (we do not add bucket for surface samples)
    if (std::abs(depthBucket) < depthInterval * 0.5)
    {
        ESP_LOGI(TAG, "Skipping sample in first depth bucket: depth=%.2f, bucket=%.2f",
                 sample->depth, depthBucket);
        return;
    }

    // Calculate tolerance of depth interval (15%)
    float tolerance = 0.15 * depthInterval;

    // Check if sample is within tolerance of the bucket depth
    float diff = std::abs(sample->depth - depthBucket);
    if (diff <= tolerance)
    {
        // Add sample to the corresponding bucket (create bucket if it doesn't exist)
        if (depthBuckets.find(depthBucket) == depthBuckets.end())
        {
            depthBuckets[depthBucket] = SampleDataList();
        }

        depthBuckets[depthBucket].push_back(sample);
        ESP_LOGI(TAG, "Added sample with depth %.2f to bucket %.2f", sample->depth, depthBucket);
    }
    else
    {
        ESP_LOGI(TAG, "Sample depth %.2f outside tolerance range of bucket %.2f (diff: %.2f, max tolerance: %.2f)",
                 sample->depth, depthBucket, diff, tolerance);
    }
}

CFieldModeCommand::SampleDataListPtr CFieldModeCommand::buildAverageSamplingByDepth( const SamplingResultsByDepth & sampleResults ) const
{
    SampleDataListPtr resultList = std::make_shared<SampleDataList>();

    // Outer loop: iterate through all depth buckets
    for (const auto& depthBucket : sampleResults) {
        float bucketDepth = depthBucket.first;
        const SampleDataList& samplesList = depthBucket.second;

        if (samplesList.empty()) {
            continue;  // Skip empty buckets
        }

        // Create new sample for averaging
        SampleData::Ptr avgSample = std::make_shared<SampleData>();

        // Set depth to bucket depth
        avgSample->depth = bucketDepth;

        // Initialize with defaults
        avgSample->temperature = -127;  // Initialize to minimal value as in ProbeSampler
        avgSample->tsStart = std::numeric_limits<time_t>::max();  // Start with max time value
        avgSample->tsEnd = 0;  // Start with min time value

        int numSamples = samplesList.size();
        ESP_LOGI(TAG, "Processing depth bucket %.2f with %d samples", bucketDepth, numSamples);

        // Inner loop: process all samples in the current bucket
        for (const auto& sample : samplesList) {
            // Track earliest start time
            if (sample->tsStart < avgSample->tsStart) {
                avgSample->tsStart = sample->tsStart;
            }

            // Track latest end time
            if (sample->tsEnd > avgSample->tsEnd) {
                avgSample->tsEnd = sample->tsEnd;
            }

            // Accumulate other values
            avgSample->temperature += sample->temperature;
            avgSample->pressure += sample->pressure;
            avgSample->pressure_voltage += sample->pressure_voltage;
            avgSample->tds += sample->tds;
            avgSample->tds_voltage += sample->tds_voltage;
            avgSample->conductivity += sample->conductivity;
            avgSample->ph_voltage += sample->ph_voltage;
            avgSample->ph += sample->ph;
            avgSample->do2_voltage += sample->do2_voltage;
            avgSample->do2 += sample->do2;
        }

        // Average the accumulated values
        avgSample->temperature /= numSamples;
        avgSample->pressure /= numSamples;
        avgSample->pressure_voltage /= numSamples;
        avgSample->tds /= numSamples;
        avgSample->tds_voltage /= numSamples;
        avgSample->conductivity /= numSamples;
        avgSample->ph_voltage /= numSamples;
        avgSample->ph /= numSamples;
        avgSample->do2_voltage /= numSamples;
        avgSample->do2 /= numSamples;

        // Add averaged sample to results
        resultList->push_back(avgSample);
    }

    return resultList;
}

void CFieldModeCommand::storeSample( const SampleData::Ptr & sample, const std::string & filename, bool append )
{
    std::ostringstream oss;
    if ( !mSerializer->serialize( sample, oss ) )
    {
        ESP_LOGI( TAG, "Failed to serialize sample" );
        return;
    }

    if( append )
    {
        if ( !mStorageService->appendData( filename, oss.str() ) )
        {
            ESP_LOGI( TAG, "Failed to append sample to %s", filename.c_str() );
        }
    }
    else
    {
        if ( !mStorageService->storeData( filename, oss.str() ) )
        {
            ESP_LOGI( TAG, "Failed to store sample to %s", filename.c_str() );
        }
    }

}

void CFieldModeCommand::storeSamples( const SampleDataListPtr & samples, const std::string & filename )
{
    bool append = false;
    for ( const auto & sample : *samples )
    {
        storeSample( sample, filename, append );
        append = true;
    }
}

std::map<std::string, std::string> CFieldModeCommand::readConfig( const std::string & filename ) const
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
        value.erase( std::remove_if( value.begin(), value.end(), ::isspace ), value.end() );

        // add key and value to configuration
        config[key] = value;
    }

    return config;
}