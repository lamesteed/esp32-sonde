#include "CSetTimeCommand.h"

#include <cstdlib>
#include <errno.h>

const char * CSetTimeCommand::ARG_TIMESTAMP = "ts";

CSetTimeCommand::CSetTimeCommand(
    const IDataPublisherService::Ptr & publisher,
    const ITimeService::Ptr & timeService,
    const ICommand::CommandArgs & args )
        : mPublisher( publisher )
        , mTimeService( timeService )
        , mArgs( args )
{

}

std::string CSetTimeCommand::getDescription() const
{
    static const char * NOTSPECIFIED = "NOT SPECIFIED";

    // retrieve ("ts"="value") from mArgs, set to "NOT SPECIFIED" if not found
    auto it = mArgs.find( ARG_TIMESTAMP );
    std::string timestamp = ( it != mArgs.end() ) ? it->second : NOTSPECIFIED;

    return "SETTIME: Set system time to: " + timestamp + " (unix timestamp)";
}

bool CSetTimeCommand::execute()
{
    // retrieve ("ts"="value") from mArgs
    auto it = mArgs.find( ARG_TIMESTAMP );
    if ( it == mArgs.end() )
    {
        std::string message = "Missing " + std::string( ARG_TIMESTAMP ) + " argument";
        mPublisher->publishData( message, true );
        return false;
    }

    // convert timestamp to long
    char *endPtr = nullptr;
    long long timestamp = strtoll( it->second.c_str(), &endPtr, 10 );
    if ( *endPtr != '\0' )
    {
        mPublisher->publishData( "Invalid timestamp format", true );
        return false;
    } else if ( errno == ERANGE )
    {
        mPublisher->publishData( "Timestamp out of range", true );
        return false;
    }

    // set system time
    bool success = mTimeService->setTime( timestamp );
    if ( !success )
    {
        mPublisher->publishData( "Failed", true );
        return false;
    }

    // publish data to client
    return mPublisher->publishData( "System time set to: " + mTimeService->getTimeAsString(), true );
}