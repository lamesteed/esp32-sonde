#include "CStoreFileCommand.h"

const char * CStoreFileCommand::ARG_FILENAME = "filename";
const char * CStoreFileCommand::ARG_DATA = "data";

CStoreFileCommand::CStoreFileCommand(
    const IStorageService::Ptr & storageService,
    const IDataPublisherService::Ptr publisher,
    const ICommand::CommandArgs & args )
        : mStorageService( storageService )
        , mPublisher( publisher )
        , mArgs( args )
{
}

std::string CStoreFileCommand::getDescription() const
{
    static const char * UNKNOWN = "UNKNOWN";

    // retrieve ("filename"="value") from mArgs, set to "UNKNOWN" if not found
    auto it = mArgs.find( ARG_FILENAME );
    std::string filename = ( it != mArgs.end() ) ? it->second : UNKNOWN;

    return "STOREFILE: Store file in storage: " + filename;
}

bool CStoreFileCommand::execute()
{
    // retrieve ("filename"="value") from mArgs
    auto it = mArgs.find( ARG_FILENAME );
    if ( it == mArgs.end() )
    {
        mPublisher->publishData( "Missing filename argument", true );
        return false;
    }

    std::string filename = it->second;

    // retrieve ("data"="value") from mArgs
    it = mArgs.find( ARG_DATA );
    if ( it == mArgs.end() )
    {
        mPublisher->publishData( "Missing data argument", true );
        return false;
    }

    std::string data = it->second;

    // store data in storage
    bool success = mStorageService->storeData( filename, data );
    if ( !success )
    {
        mPublisher->publishData( "Failed to store file: " + filename, true );
        return false;
    }

    // publish success message to client
    return mPublisher->publishData( "File stored successfully: " + filename, true );
}
