#include "CGetFileCommand.h"

const char * CGetFileCommand::ARG_FILENAME = "filename";

CGetFileCommand::CGetFileCommand(
    const IStorageService::Ptr & storageService,
    const IDataPublisherService::Ptr publisher,
    const ICommand::CommandArgs & args )
        : mStorageService( storageService )
        , mPublisher( publisher )
        , mArgs( args )
{
}

std::string CGetFileCommand::getDescription() const
{
    static const char * UNKNOWN = "UNKNOWN";

    // retrieve ("filename"="value") from mArgs, set to "UNKNOWN" if not found
    auto it = mArgs.find( ARG_FILENAME );
    std::string filename = ( it != mArgs.end() ) ? it->second : UNKNOWN;

    return "GETFILE: Get file from storage: " + filename;
}

bool CGetFileCommand::execute()
{
    // retrieve ("filename"="value") from mArgs
    auto it = mArgs.find( ARG_FILENAME );
    if ( it == mArgs.end() )
    {
        mPublisher->publishData( "Missing filename argument", true );
        return false;
    }

    std::string filename = it->second;
    std::string fileData;

    // read data from storage using input stream
    IInputStream::Ptr inputStream = mStorageService->getInputStream( filename );
    if ( !inputStream )
    {
        mPublisher->publishData( "Failed to get input stream for file: " + filename, true );
        return false;
    }

    return mPublisher->publishData( inputStream );
}