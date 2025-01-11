#include "CRebootCommand.h"
#include "IDataPublisherService.h"
#include "IRebootable.h"

CRebootCommand::CRebootCommand(
    const IDataPublisherService::Ptr & publisher,
    const IRebootable::Ptr & rebootable )
        : mPublisher( publisher )
        , mRebootable( rebootable )
{

}

std::string CRebootCommand::getDescription() const
{
    return "REBOOT: Reboots Sonde";
}

bool CRebootCommand::execute()
{
    mRebootable->reboot();

    mPublisher->publishData( "Rebooting Sonde", true );

    return true;
}