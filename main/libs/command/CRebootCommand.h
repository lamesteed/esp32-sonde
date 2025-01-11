#ifndef CREBOOTCOMMAND_H
#define CREBOOTCOMMAND_H

#include "ICommand.h"
#include "IDataPublisherService.h"
#include "IRebootable.h"

class CRebootCommand : public ICommand
{
public:
    CRebootCommand(
        const IDataPublisherService::Ptr & publisher,
        const IRebootable::Ptr & rebootable );
    virtual ~CRebootCommand() = default;

private:
    // ICommand interface
    virtual std::string getDescription() const override;
    virtual bool execute() override;

    IDataPublisherService::Ptr mPublisher;
    IRebootable::Ptr mRebootable;
};

#endif // CREBOOTCOMMAND_H