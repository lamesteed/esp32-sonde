#ifndef ISTORAGESERVICE_H
#define ISTORAGESERVICE_H

#include <memory>
#include <list>
#include <string>

class IStorageService
{
public:
    using Ptr = std::shared_ptr<IStorageService>;
    using FileList = std::list<std::string>;

    virtual ~IStorageService() = default;

    virtual bool start() = 0;
    virtual bool stop() = 0;

    virtual bool getStatus() = 0;

    virtual bool listFiles( FileList & outFiles ) = 0;
    virtual bool readData( const std::string & filename, std::string & outData ) = 0;
    virtual bool storeData( const std::string & filename, const std::string & inData) = 0;
    virtual bool appendData( const std::string & filename, const std::string & inData) = 0;
};

#endif // ISTORAGESERVICE_H