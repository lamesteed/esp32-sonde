#include "CListFilesCommand.h"

#include <sstream>

CListFilesCommand::CListFilesCommand(
    const IStorageService::Ptr & storageService,
    const IDataPublisherService::Ptr publisher )
        : mStorageService( storageService )
        , mPublisher( publisher )
{

}

std::string CListFilesCommand::getDescription() const
{
    return "LISTFILES: Lists sampling data files in storage";
}

bool CListFilesCommand::execute()
{
    // list files
    IStorageService::FileList files;
    std::ostringstream oss;
    if ( mStorageService->listFiles( files ) )
    {
        // prepare message (use comma as separator)
        for ( const auto & file : files )
        {
            oss << file << ",";
        }

        // publish message
        mPublisher->publishData( oss.str(), true );
         return true;
    } else
    {
        mPublisher->publishData( "Failed to list files in storage", true );
        return false;
    }
}