#ifndef CWATCHDOG_H
#define CWATCHDOG_H

#include "IWatchdog.h"
#include "ITimeService.h"


class CWatchdog : public IWatchdog
{
public:
    /// @brief Constructor
    CWatchdog( const ITimeService::Ptr & timeService );

    /// @brief Destructor
    virtual ~CWatchdog();

private:
    /// IWatchdog interface

    /// @brief Start the watchdog timer
    /// @param expirationTimeSec Time in seconds after which the timer will expire
    virtual void start( int expirationTimeSec ) override;

    /// @brief  Reset the watchdog timer
    virtual void reset() override;

    /// @brief Test if the watchdog timer has expired
    /// @return true if the timer has expired, false otherwise
    virtual bool isExpired() override;

private:

    ITimeService::Ptr mTimeService; ///< Time service instance
    time_t mStartTime;              ///< Start time in seconds
    int  mExpirationIntervalSec;    ///< Expiration time in seconds
    bool mIsRunning;                ///< Flag indicating if the watchdog is running
};


#endif // CWATCHDOG_H