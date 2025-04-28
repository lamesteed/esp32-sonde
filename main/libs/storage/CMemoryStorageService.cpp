#include "CMemoryStorageService.h"
#include "esp_log.h"

#include <sstream>

const char * CMemoryStorageService::TAG = "CMemoryStorageService";

CMemoryStorageService::CMemoryStorageService( const IStorageService::Ptr & underlying )
    : mUnderlying( underlying )
    , mBypassMemoryStorage( false )
{
    ESP_LOGI( TAG, "Instance created" );
}

CMemoryStorageService::~CMemoryStorageService()
{
    ESP_LOGI( TAG, "Instance destroyed" );
}

bool CMemoryStorageService::start()
{
    mBypassMemoryStorage = mUnderlying->start();
    if ( mBypassMemoryStorage )
    {
        ESP_LOGI( TAG, "start() - underlying storage available, disabling in-memory decorator" );
    }
    else
    {
        ESP_LOGI( TAG, "start() - underlying storage not available, use in-memory decorator (no persistency upon reboot)" );

        // Initialize in-memory storage

        {
            // Generatate sampler.cfg calibration config file content
            // use sstream to generate multiline key-value pairs
            std::ostringstream oss;
            oss << "NUMBER_OF_SAMPLES=10" << std::endl;
            oss << "TDS_CONVERSION_FACTOR_A=434.8" << std::endl;
            oss << "TDS_CONVERSION_FACTOR_B=0" << std::endl;
            oss << "PRESSURE_CONVERSION_FACTOR_A=25" << std::endl;
            oss << "PRESSURE_CONVERSION_FACTOR_B=-12.5" << std::endl;
            oss << "PH_CONVERSION_FACTOR_A=1" << std::endl;
            oss << "PH_CONVERSION_FACTOR_B=0" << std::endl;
            oss << "DO_CONVERSION_FACTOR_A=1" << std::endl;
            oss << "DO_CONVERSION_FACTOR_B=0" << std::endl;
            std::lock_guard<std::mutex> lock( mStorageMutex );
            mStorage["sampler.cfg" ] = oss.str();
        }

        {
            // Generatate metadata.cfg calibration config file content
            // use sstream to generate multiline key-value pairs
            std::ostringstream oss;
            oss << "DATASET_NAME=UNKNOWN" << std::endl;
            oss << "LOCATION_ID=UNKNOWN_LOCATION_ID" << std::endl;
            oss << "LOCATION_NAME=UNKNOWN_LOCATION_NAME" << std::endl;
            oss << "LOCATION_TYPE=Lake/Pond" << std::endl;
            oss << "GPS_LATITUDE=0.0" << std::endl;
            oss << "GPS_LONGITUDE=0.0" << std::endl;
            oss << "GPS_ACCURACY=10.0" << std::endl;
            oss << "GPS_ACCURACY_UNIT=m" << std::endl;
            oss << "GPS_SYSTEM_NAME=WGS84" << std::endl;
            oss << "SAMPLING_STEP=1.0" << std::endl;
            oss << "SAMPLING_INTERVAL_MSEC=5000" << std::endl;
            oss << "SAMPLING_MAX_DURATION_SEC=0" << std::endl;
            std::lock_guard<std::mutex> lock( mStorageMutex );
            mStorage["metadata.cfg"] = oss.str();
        }
    }

    return mBypassMemoryStorage;
}

bool CMemoryStorageService::stop()
{
    return mUnderlying->stop();
}

bool CMemoryStorageService::getStatus()
{
    return mUnderlying->getStatus();
}

bool CMemoryStorageService::listFiles( FileList & outFiles )
{
    if ( mBypassMemoryStorage )
    {
        return mUnderlying->listFiles( outFiles );
    }
    else
    {
        ESP_LOGI( TAG, "listFiles() - listing files in memory" );
        std::lock_guard<std::mutex> lock( mStorageMutex );
        for ( const auto & file : mStorage )
        {
            outFiles.push_back( file.first );
        }
        return true;
    }
}

bool CMemoryStorageService::readData( const std::string & filename, std::string & outData )
{
    if ( mBypassMemoryStorage )
    {
        return mUnderlying->readData( filename, outData );
    }
    else
    {
        ESP_LOGI( TAG, "readData() - reading data from file: %s", filename.c_str() );
        std::lock_guard<std::mutex> lock( mStorageMutex );
        auto it = mStorage.find( filename );
        if ( it != mStorage.end() )
        {
            outData = it->second;
            return true;
        }
        else
        {
            ESP_LOGE( TAG, "readData() - file not found in memory: %s", filename.c_str() );
            return false;
        }
    }
}

bool CMemoryStorageService::storeData( const std::string & filename, const std::string & inData )
{
    if ( mBypassMemoryStorage )
    {
        return mUnderlying->storeData( filename, inData );
    }
    else
    {
        ESP_LOGI( TAG, "storeData() - storing data to file: %s", filename.c_str() );
        std::lock_guard<std::mutex> lock( mStorageMutex );
        mStorage[filename] = inData;
        return true;
    }
}

bool CMemoryStorageService::appendData( const std::string & filename, const std::string & inData )
{
    if ( mBypassMemoryStorage )
    {
        return mUnderlying->appendData( filename, inData );
    }
    else
    {
        ESP_LOGI( TAG, "appendData() - appending data to file: %s", filename.c_str() );
        std::lock_guard<std::mutex> lock( mStorageMutex );
        //try fetch data from memory, create new entry if not found or append to existing one if found
        auto it = mStorage.find( filename );
        if ( it != mStorage.end() )
        {
            it->second += "\n";
            it->second += inData;
        }
        else
        {
            mStorage[filename] = inData;
        }
        return true;
    }
}