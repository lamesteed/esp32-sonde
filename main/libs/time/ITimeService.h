#ifndef ITIMESERVICE_H
#define ITIMESERVICE_H

#include <memory>
#include <string>

/// @brief System time service interface ( abstracts calls to system time functions )
class ITimeService
{
public:
    using Ptr = std::shared_ptr<ITimeService>;
    /// @brief Virtual destructor
    virtual ~ITimeService() = default;

    /// @brief Get current system time as string
    /// @return Current system time as string
    virtual std::string GetTimeAsString() = 0;

    /// @brief Set system time
    /// @param uniixTimestamp unix timestamp to set
    /// @return true if time set successfully, false otherwise
    virtual bool SetTime( long uniixTimestamp ) = 0;
};


#endif // ITIMESERVICE_H