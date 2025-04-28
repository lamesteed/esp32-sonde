#include "CStringInputStream.h"
#include "esp_log.h"
#include <string.h>

const char * CStringInputStream::TAG = "CStringInputStream";

CStringInputStream::CStringInputStream( const std::string & data )
    : mData( data )
    , mPosition( 0 )
    , mSize( data.size() )
{
    ESP_LOGI( TAG, "Instance created, position: %d, size: %d", mPosition, mSize );
}

CStringInputStream::~CStringInputStream()
{
    ESP_LOGI( TAG, "Instance destroyed" );
    close();
}

size_t CStringInputStream::read( uint8_t *buffer, size_t size )
{
    if ( mPosition >= mSize )
    {
        ESP_LOGE( TAG, "read() - end of stream reached" );
        return 0;
    }

    size_t bytesToRead = std::min( size, mSize - mPosition );
    memcpy( buffer, mData.c_str() + mPosition, bytesToRead );
    mPosition += bytesToRead;

    ESP_LOGI( TAG, "read() - read %d bytes, position: %d", bytesToRead, mPosition );
    return bytesToRead;
}

bool CStringInputStream::eof() const
{
    return mPosition >= mSize;
}

void CStringInputStream::close()
{
    ESP_LOGI( TAG, "close() - stream closed" );
}