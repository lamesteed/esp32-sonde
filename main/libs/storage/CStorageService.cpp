#include "CStorageService.h"

#define DEBUG_ESP_SD
#include "SD_MMC.h"
#include "esp_log.h"

#include <sstream>

// constants definition
const char * CStorageService::TAG = "CStorageService";
const char * CStorageService::SENTRY226_DIR = "/sentry226";

CStorageService::CStorageService()
    : mInitialized( false )
{
    ESP_LOGI( TAG, "Instance created" );
}

CStorageService::~CStorageService()
{
    stop();
    ESP_LOGI( TAG, "Instance destroyed" );
}

bool CStorageService::start()
{
    ESP_LOGI( TAG, "start() - starting storage service" );

    if ( mInitialized )
    {
        ESP_LOGW( TAG, "start() - service already started" );
        return true;
    }

    // Mount SD card
    if ( !SD_MMC.begin() )
    {
        ESP_LOGE( TAG, "start() - failed to mount SD card" );
        return false;
    }

    // Check if SD card present and if yes - print card type
    std::string cardTypeStr;
    switch( SD_MMC.cardType() )
    {
        case CARD_NONE:
            ESP_LOGE( TAG, "start() - No SD card found" );
            return false;
        case CARD_SD:
            cardTypeStr = "SD";
            break;
        case CARD_MMC:
            cardTypeStr = "MMC";
            break;
        case CARD_SDHC:
            cardTypeStr = "SDHC";
            break;
        case CARD_UNKNOWN:
        default:
            cardTypeStr = "Unknown";
            break;
    }

    // Get SD card size and print card info
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    uint64_t totalBytes = SD_MMC.totalBytes() / (1024 * 1024);
    uint64_t usedBytes = SD_MMC.usedBytes() / (1024 * 1024);

    ESP_LOGI( TAG,
              "Card type: %s, size: %lluMB, Total space: %lluMB, Used space: %lluMB",
              cardTypeStr.c_str(),
              cardSize,
              totalBytes,
              usedBytes );

    // check if sentry226 directory exists, if not create it
    if ( !SD_MMC.exists( SENTRY226_DIR ) )
    {
        ESP_LOGI( TAG, "start() - %s directory not found, creating", SENTRY226_DIR );

        if ( !SD_MMC.mkdir( SENTRY226_DIR ) )
        {
            ESP_LOGE( TAG, "start() - failed to create %s directory", SENTRY226_DIR );
            return false;
        }
    }

    mInitialized = true;
    ESP_LOGI( TAG, "start() - storage service started" );
    return true;
}

bool CStorageService::stop()
{
    ESP_LOGI( TAG, "stop() - stopping storage service" );

    if ( !mInitialized )
    {
        ESP_LOGW( TAG, "stop() - service not started" );
        return true;
    }

    // unmount SD card
    SD_MMC.end();

    mInitialized = false;
    ESP_LOGI( TAG, "stop() - storage service stopped" );
    return true;
}

bool CStorageService::getStatus()
{
    if (!mInitialized) {
        ESP_LOGE(TAG, "Storage service is not initialized, this=%p", this);
        return false;
    }
    const char* mountpoint = SD_MMC.mountpoint();
    ESP_LOGI(TAG, "Storage initialized: %d, this=%p, mount = --%s--'", mInitialized, this, mountpoint ? mountpoint : "null");
    return mInitialized;
}

bool CStorageService::listFiles( FileList & outFiles )
{
    ESP_LOGI( TAG, "listFiles() - listing files in %s directory", SENTRY226_DIR );

    if ( !mInitialized )
    {
        ESP_LOGE( TAG, "listFiles() - service not initialized" );
        return false;
    }

    // open /sentry226 directory
    File root = SD_MMC.open( SENTRY226_DIR );
    if ( !root )
    {
        ESP_LOGE( TAG, "listFiles() - failed to open %s directory", SENTRY226_DIR );
        return false;
    }

    // list all files in /sentry226 directory
    File file = root.openNextFile();
    while( file )
    {
        outFiles.push_back( file.name() );
        file.close();
        file = root.openNextFile();
    }

    return true;
}

bool CStorageService::readData( const std::string & filename, std::string & outData )
{
    ESP_LOGI( TAG, "readData() - reading data from file: %s", filename.c_str() );

    if ( !mInitialized )
    {
        ESP_LOGE( TAG, "readData() - service not initialized" );
        return false;
    }

    // construct filename (concatenate directory and filename)
    std::string fullFilename = SENTRY226_DIR;
    fullFilename += "/";
    fullFilename += filename;

    // open file for reading
    File file = SD_MMC.open( fullFilename.c_str(), FILE_READ );
    if ( !file )
    {
        ESP_LOGE( TAG, "readData() - failed to open file for reading" );
        return false;
    }

    // read data from file in chunks of 256 bytes until EOF
    // append data read to temporary string buffer
    std::ostringstream oss;
    char buffer[256];
    while( file.available() )
    {
        int bytesRead = file.read( (uint8_t *)buffer, sizeof(buffer) );
        if ( bytesRead < 0 )
        {
            ESP_LOGE( TAG, "readData() - failed to read data from file" );
            file.close();
            return false;
        }
        oss.write( buffer, bytesRead );
    }

    // close file
    file.close();

    // copy data from string buffer to output string
    outData = oss.str();
    return true;
}

bool CStorageService::storeData( const std::string & filename, const std::string & inData )
{
    ESP_LOGI( TAG, "storeData() - storing data to file: %s", filename.c_str() );

    if ( !mInitialized )
    {
        ESP_LOGE( TAG, "storeData() - service not initialized" );
        return false;
    }

    // construct filename (concatenate directory and filename)
    std::string fullFilename = SENTRY226_DIR;
    fullFilename += "/";
    fullFilename += filename;

    // open file for writing (will recreate file if it already exists)
    File file = SD_MMC.open( fullFilename.c_str(), FILE_WRITE, true );
    if ( !file )
    {
        ESP_LOGE( TAG, "storeData() - failed to open file for writing" );
        return false;
    }

    // write data to file
    size_t bytesWritten = file.write( (const uint8_t *)inData.c_str(), inData.size() );
    if ( bytesWritten != inData.size() )
    {
        ESP_LOGE( TAG, "storeData() - failed to write data to file" );
        file.close();
        return false;
    }

    // close file
    file.close();
    return true;
}

bool CStorageService::appendData( const std::string & filename, const std::string & inData )
{
    ESP_LOGI( TAG, "appendData() - appending data to file: %s", filename.c_str() );

    if ( !mInitialized )
    {
        ESP_LOGE( TAG, "appendData() - service not initialized" );
        return false;
    }

    // construct filename (concatenate directory and filename)
    std::string fullFilename = SENTRY226_DIR;
    fullFilename += "/";
    fullFilename += filename;

    // open file for appending
    File file = SD_MMC.open( fullFilename.c_str(), FILE_APPEND );
    if ( !file )
    {
        ESP_LOGE( TAG, "appendData() - failed to open file for appending" );
        return false;
    }

    // write data to file
    size_t bytesWritten = file.write( (const uint8_t *)inData.c_str(), inData.size() );
    if ( bytesWritten != inData.size() )
    {
        ESP_LOGE( TAG, "appendData() - failed to append data to file" );
        file.close();
        return false;
    }

    // close file
    file.close();
    return true;
}