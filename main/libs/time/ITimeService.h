#ifndef ITIMESERVICE_H
#define ITIMESERVICE_H

#include <memory>
#include <string>

/// @brief System time service interface ( abstracts calls to system time functions )
class ITimeService
{
public:
    using Ptr = std::shared_ptr<ITimeService>;
    /// @brief virtual destructor
    virtual ~ITimeService() = default;

    /// @brief Get current system time as string
    /// @return Current system time as string
    virtual std::string getTimeAsString(const std::string &format = "%Y-%m-%d %H:%M:%S") = 0;

    /// @brief Set system time
    /// @param msec unix timestamp in milliseconds to set
    /// @return true if time set successfully, false otherwise
    virtual bool setTime( long long msec ) = 0;

    /// @brief Get current system time in milliseconds
    /// @return unix timestamp in milliseconds
    virtual time_t getTime() = 0;

    /// @brief convert milliseconds to string using specified format
    /// @param msec unix timestamp in milliseconds
    /// @param format string format to use for conversion
    /// @return string representation of the time in specified format
    virtual std::string toString( long long msec, const std::string &format = "%Y-%m-%d %H:%M:%S" ) = 0;
};


#endif // ITIMESERVICE_H