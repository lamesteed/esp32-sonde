#ifndef IWATCHDOG_H
#define IWATCHDOG_H

#include <memory>

/// @brief Interface for operation watchdog
class IWatchdog
{
public:
    using Ptr = std::shared_ptr<IWatchdog>;

    /// @brief Virtual destructor
    virtual ~IWatchdog() = default;

    /// @brief Start the watchdog timer
    /// @param expirationTimeSec Time in seconds after which the timer will expire
    ///        Note: Watchdog will never expire if this function is not called or if 0 is passed
    virtual void start( int expirationTimeSec ) = 0;

    /// @brief Reset the watchdog timer
    ///        Note: this function should be called periodically to prevent the timer from expiring
    virtual void reset() = 0;

    /// @brief Test if the watchdog timer has expired
    /// @return true if the timer has expired, false otherwise
    ///         Note: this function should be called periodically to check if the timer has expired
    virtual bool isExpired() = 0;
};

#endif // IWATCHDOG_H