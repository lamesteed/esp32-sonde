#include "CSystemTimeService.h"
#include "esp_log.h"

#include <sys/time.h>
#include <errno.h>
#include <string.h>

const char * CSystemTimeService::TAG = "CSystemTimeService";

CSystemTimeService::CSystemTimeService()
{
    ESP_LOGI( TAG, "Instance created, System Time: %s", GetTimeAsString().c_str() );
}

CSystemTimeService::~CSystemTimeService()
{
    ESP_LOGI( TAG, "Instance destroyed" );
}

std::string CSystemTimeService::GetTimeAsString()
{
    struct timeval tv;
    gettimeofday( &tv, NULL );
    time_t nowtime = tv.tv_sec;
    struct tm *nowtm = localtime( &nowtime );
    char tmbuf[64];
    strftime( tmbuf, sizeof( tmbuf ), "%Y-%m-%d %H:%M:%S", nowtm );
    return std::string( tmbuf );
}

bool CSystemTimeService::SetTime( long uniixTimestamp )
{
    struct timeval tv;
    tv.tv_sec = uniixTimestamp;
    tv.tv_usec = 0;
    if ( settimeofday( &tv, NULL ) < 0 )
    {
        ESP_LOGE( TAG, "SetTime() - failed to set system time. Reason: %s", strerror( errno ) );
        return false;
    }
    ESP_LOGI( TAG, "SetTime() - System Time set to: %s", GetTimeAsString().c_str() );
    return true;
}