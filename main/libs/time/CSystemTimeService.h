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

    // ITimeService interface
    /// @brief Get current system time as string to be used in filenames
    /// @return Current system time as string
    std::string GetTimeAsFilename();

    private:
    // ITimeService interface
    /// @brief Get current system time as string
    /// @return Current system time as string
    std::string GetTimeAsString() override;

    /// @brief Set system time
    /// @param msec unix timestamp in milliseconds to set
    /// @return true if time set successfully, false otherwise
    bool SetTime( long long msec ) override;

private:
    static const char * TAG;    ///< Logger tag
};


#endif // CSYSTEMTIMESERVICE_H