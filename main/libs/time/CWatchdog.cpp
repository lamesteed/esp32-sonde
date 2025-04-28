#include "CWatchdog.h"

CWatchdog::CWatchdog( const ITimeService::Ptr & timeService )
    : mTimeService( timeService )
    , mStartTime( 0 )
    , mExpirationIntervalSec( 0 )
    , mIsRunning( false )
{

}

CWatchdog::~CWatchdog()
{
    mIsRunning = false;
}

void CWatchdog::start( int expirationTimeSec )
{
    if ( expirationTimeSec <= 0 )
    {
        mIsRunning = false;
        return;
    }

    mStartTime = mTimeService->getTime();
    mExpirationIntervalSec = expirationTimeSec;
    mIsRunning = true;
}

void CWatchdog::reset()
{
    if ( mIsRunning )
    {
        mStartTime = mTimeService->getTime();
    }
}

bool CWatchdog::isExpired()
{
    if ( !mIsRunning )
    {
        return false;
    }

    if ( ( mTimeService->getTime() - mStartTime ) > mExpirationIntervalSec )
    {
        return true;
    }

    return false;
}