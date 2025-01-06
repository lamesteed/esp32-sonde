#ifndef CREBOOTCOMMAND_H
#define CREBOOTCOMMAND_H

#include "ICommand.h"

class IDataPublisherService;
class IRebootable;

class CRebootCommand : public ICommand
{
public:
    CRebootCommand( IDataPublisherService & publisher, IRebootable & rebootable );
    virtual ~CRebootCommand();

private:
    // ICommand interface
    virtual std::string getDescription() const override;
    virtual bool execute() override;

    IDataPublisherService & mPublisher;
    IRebootable & mRebootable;
};

#endif // CREBOOTCOMMAND_H