#include "CRebootCommand.h"
#include "IDataPublisherService.h"
#include "IRebootable.h"

CRebootCommand::CRebootCommand( IDataPublisherService & publisher, IRebootable & rebootable )
    : mPublisher( publisher )
    , mRebootable( rebootable )
{

}

CRebootCommand::~CRebootCommand()
{

}

std::string CRebootCommand::getDescription() const
{
    return "REBOOT: Reboot the device";
}

bool CRebootCommand::execute()
{
    mRebootable.reboot();

    mPublisher.publishData( "Rebooting Sonde", true );

    return true;
}