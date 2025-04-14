#include "CSystemTimeService.h"
#include "esp_log.h"

#include <sys/time.h>
#include <errno.h>
#include <string.h>

const char * CSystemTimeService::TAG = "CSystemTimeService";

CSystemTimeService::CSystemTimeService()
{
    ESP_LOGI( TAG, "Instance created, System Time: %s", GetTimeAsString("%Y-%m-%d %H:%M:%S").c_str() );
}

CSystemTimeService::~CSystemTimeService()
{
    ESP_LOGI( TAG, "Instance destroyed" );
}

std::string CSystemTimeService::GetTimeAsString(const std::string &format)
{
    struct timeval tv;
    gettimeofday( &tv, NULL );
    time_t nowtime = tv.tv_sec;
    struct tm *nowtm = localtime( &nowtime );
    char tmbuf[64];
    strftime( tmbuf, sizeof( tmbuf ), format.c_str(), nowtm );
    return std::string( tmbuf );
}

bool CSystemTimeService::SetTime( long long msec )
{
    struct timeval tv;
    tv.tv_sec  = msec / 1000;            // Convert milliseconds to seconds
    tv.tv_usec = ( msec % 1000 ) * 1000; // Convert remainder to microseconds
    if ( settimeofday( &tv, NULL ) < 0 )
    {
        ESP_LOGE( TAG, "SetTime() - failed to set system time. Reason: %s", strerror( errno ) );
        return false;
    }
    ESP_LOGI( TAG, "SetTime() - System Time set to: %s", GetTimeAsString().c_str() );
    return true;
}