#ifndef CMEMORYSTORAGESERVICE_H
#define CMEMORYSTORAGESERVICE_H

#include "IStorageService.h"
#include <map>
#include <mutex>

/// @brief Decorator that acts as simple in-memory storage service in case SD card is not available
/// No real persistance will be obviously possible, but within the same power cycle decorator will be able
/// to store and provide stored data using simple in-memory map filename is key, data is value in string format
class CMemoryStorageService : public IStorageService
{
    public:
        /// @brief Default constructor
        CMemoryStorageService( const IStorageService::Ptr & underlying );

        /// @brief Destructor
        virtual ~CMemoryStorageService();

    private:
        // IStorageService interface
        virtual bool start() override;
        virtual bool stop() override;
        virtual bool getStatus() override;

        virtual bool listFiles( FileList & outFiles ) override;
        virtual bool readData( const std::string & filename, std::string & outData ) override;
        virtual bool storeData( const std::string & filename, const std::string & inData) override;
        virtual bool appendData( const std::string & filename, const std::string & inData) override;

    private:
        static const char * TAG; ///< Logger tag

        IStorageService::Ptr mUnderlying; ///< Underlying storage service
        std::map<std::string, std::string> mStorage; ///< In-memory storage map
        std::mutex mStorageMutex; ///< Mutex for thread safety when accessing mStorage
        bool mBypassMemoryStorage; ///< Flag to bypass memory storage (if underlying storage is available)
};

#endif // CMEMORYSTORAGESERVICE_H