#ifndef CSYSTEMTIMESERVICE_H
#define CSYSTEMTIMESERVICE_H

#include "ITimeService.h"

/// @brief System time service implementation
class CSystemTimeService : public ITimeService
{
public:
    /// @brief Constructor
    CSystemTimeService();

    /// @brief Destructor
    virtual ~CSystemTimeService();

    private:
    // ITimeService interface
    /// @brief Get current system time as string
    /// @return Current system time as string
    std::string getTimeAsString(const std::string &format = "%Y-%m-%d %H:%M:%S") override;

    /// @brief Set system time
    /// @param msec unix timestamp in milliseconds to set
    /// @return true if time set successfully, false otherwise
    bool setTime( long long msec ) override;

    /// @brief Get current system time in milliseconds
    /// @return unix timestamp in milliseconds
    virtual long long getTime() override;

    /// @brief convert milliseconds to string using specified format
    /// @param msec unix timestamp in milliseconds
    /// @param format string format to use for conversion
    /// @return string representation of the time in specified format
    virtual std::string toString( long long msec, const std::string &format = "%Y-%m-%d %H:%M:%S" ) override;

    static const char * TAG;    ///< Logger tag
};


#endif // CSYSTEMTIMESERVICE_H