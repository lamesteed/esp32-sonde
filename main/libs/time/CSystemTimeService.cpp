#include "CSystemTimeService.h"
#include "esp_log.h"

#include <sys/time.h>
#include <errno.h>
#include <string.h>

const char * CSystemTimeService::TAG = "CSystemTimeService";

CSystemTimeService::CSystemTimeService()
{
    ESP_LOGI( TAG, "Instance created, System Time: %s", getTimeAsString("%Y-%m-%d %H:%M:%S").c_str() );
}

CSystemTimeService::~CSystemTimeService()
{
    ESP_LOGI( TAG, "Instance destroyed" );
}

std::string CSystemTimeService::getTimeAsString(const std::string &format)
{
    struct timeval tv;
    gettimeofday( &tv, NULL );
    time_t nowtime = tv.tv_sec;
    struct tm *nowtm = localtime( &nowtime );
    char tmbuf[64];
    strftime( tmbuf, sizeof( tmbuf ), format.c_str(), nowtm );
    return std::string( tmbuf );
}

bool CSystemTimeService::setTime( long long msec )
{
    struct timeval tv;
    tv.tv_sec  = msec / 1000;            // Convert milliseconds to seconds
    tv.tv_usec = ( msec % 1000 ) * 1000; // Convert remainder to microseconds
    if ( settimeofday( &tv, NULL ) < 0 )
    {
        ESP_LOGE( TAG, "SetTime() - failed to set system time. Reason: %s", strerror( errno ) );
        return false;
    }
    ESP_LOGI( TAG, "SetTime() - System Time set to: %s", getTimeAsString().c_str() );
    return true;
}

time_t CSystemTimeService::getTime()
{
    struct timeval tv;
    gettimeofday( &tv, NULL );
    return tv.tv_sec; // Return seconds since epoch
}

std::string CSystemTimeService::toString( time_t seconds, const std::string & format )
{
    struct tm *nowtm = localtime(&seconds); // Get local time from seconds
    char tmbuf[64];
    strftime(tmbuf, sizeof(tmbuf), format.c_str(), nowtm); // Format the time
    return std::string(tmbuf);
}