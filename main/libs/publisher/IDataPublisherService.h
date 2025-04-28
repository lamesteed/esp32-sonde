#ifndef IDATAPUBLISHERSERVICE_H
#define IDATAPUBLISHERSERVICE_H

#include<memory>
#include "IInputStream.h"

class ICommandListener;

class IDataPublisherService
{
public:
    using Ptr =  std::shared_ptr<IDataPublisherService>;

    /// @brief Virtual destructor
    virtual ~IDataPublisherService() = default;

    /// @brief Initializes and starts data publishing service
    /// @return true if service started successfully, false otherwise
    virtual bool start() = 0;

    /// @brief Cleans up and stops data publishing service
    /// @return true if service stopped successfully, false otherwise
    virtual bool stop() = 0;

    /// @brief Performs data publishing for consumers, data considered as published (consumed) after this call
    /// @param data string-serilized data to be published for consumer
    /// @return true if data published successfully, false otherwise
    virtual bool publishData( const std::string & data, bool sendEOD ) = 0;

    virtual bool publishData( const IInputStream::Ptr & inputStream ) = 0;

    virtual void setNotificationListener( ICommandListener * listener ) = 0;
};

#endif // IDATAPUBLISHERSERVICE_H