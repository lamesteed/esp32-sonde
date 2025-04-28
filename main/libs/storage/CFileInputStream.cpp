#include "CFileInputStream.h"
#include "esp_log.h"

const char * CFileInputStream::TAG = "CFileInputStream";

CFileInputStream::CFileInputStream( const File & file )
    : mFile( file )
    , mPosition( 0 )
    , mSize( file.size() )
{
    ESP_LOGI( TAG, "Instance created, position: %d, size: %d", mPosition, mSize );
}

CFileInputStream::~CFileInputStream()
{
    ESP_LOGI( TAG, "Instance destroyed" );
    close();
}

size_t CFileInputStream::read( uint8_t *buffer, size_t size )
{
    if ( !mFile )
    {
        ESP_LOGE( TAG, "read() - file is not open" );
        return 0;
    }

    size_t readBytes = mFile.read( buffer, size );
    mPosition += readBytes;
    ESP_LOGI( TAG, "read() - read %d bytes, position: %d", readBytes, mPosition );
    return readBytes;
}

bool CFileInputStream::eof() const
{
    return mPosition >= mSize;
}

void CFileInputStream::close()
{
    if ( mFile )
    {
        mFile.close();
        ESP_LOGI( TAG, "close() - file closed" );
    }
}