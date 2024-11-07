#ifndef IDATAPUBLISHERSERVICE_H
#define IDATAPUBLISHERSERVICE_H

#include <list>
#include <string>

class IDataPublisherService
{
public:
    typedef std::string SampleData;
    typedef std::list<SampleData> SampleDataList;

    /// @brief Virtual destructor
    virtual ~IDataPublisherService()
    {
    }

    /// @brief Performs data publishing for consumers, data considered as published (consumed) after this call
    /// @param data List of data samples to be published for consumer
    /// @return true if data published successfully, false otherwise
    virtual bool publishData( const SampleDataList & data ) = 0;
};

#endif // IDATAPUBLISHERSERVICE_H