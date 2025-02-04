#ifndef CSTORAGESERVICE_H
#define CSTORAGESERVICE_H

#include "IStorageService.h"

class CStorageService : public IStorageService
{
public:
    CStorageService();
    virtual ~CStorageService();

private:
    // IStorageService interface
    virtual bool start() override;
    virtual bool stop() override;

    virtual bool getStatus() override;

    virtual bool listFiles( FileList & outFiles ) override;
    virtual bool readData( const std::string & filename, std::string & outData ) override;
    virtual bool storeData( const std::string & filename, const std::string & inData) override;

private:
    static const char * TAG; ///< Logger tag
    static const char * SENTRY226_DIR; ///< Directory name for sentry226 data
    bool mInitialized;       ///< Flag indicating if service is initialized
};

#endif // CSTORAGESERVICE_H