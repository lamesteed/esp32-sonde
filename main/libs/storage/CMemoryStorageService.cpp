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

        // Generatate sampler.cfg calibration config file content
        // use sstream to generate multiline key-value pairs
        std::ostringstream oss;
        oss << "NUMBER_OF_SAMPLES=10" << std::endl;
        oss << "TDS_CONVERSION_FACTOR_A=434.8" << std::endl;
        oss << "TDS_CONVERSION_FACTOR_B=0" << std::endl;
        oss << "PRESSURE_CONVERSION_FACTOR_A=25" << std::endl;
        oss << "PRESSURE_CONVERSION_FACTOR_B=-12.5" << std::endl;
        {
            std::lock_guard<std::mutex> lock( mStorageMutex );
            mStorage["sampler.cfg" ] = oss.str();
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
    else
    {
        return mUnderlying->readData( filename, outData );
    }
}

bool CMemoryStorageService::storeData( const std::string & filename, const std::string & inData )
{
    if ( mBypassMemoryStorage )
    {
        std::lock_guard<std::mutex> lock( mStorageMutex );
        mStorage[filename] = inData;
        return true;
    }
    else
    {
        return mUnderlying->storeData( filename, inData );
    }
}