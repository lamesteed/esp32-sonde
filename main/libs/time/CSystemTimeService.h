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
    std::string GetTimeAsString() override;

    /// @brief Set system time
    /// @param uniixTimestamp unix timestamp to set
    /// @return true if time set successfully, false otherwise
    bool SetTime( long uniixTimestamp ) override;

private:
    static const char * TAG;    ///< Logger tag
};


#endif // CSYSTEMTIMESERVICE_H