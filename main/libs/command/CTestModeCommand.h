#ifndef CTESTMODECOMMAND_H
#define CTESTMODECOMMAND_H

#include "ICommand.h"

class IDataPublisherService;

class CTestModeCommand : public ICommand
{
public:
    CTestModeCommand( IDataPublisherService & publisher, int samplesCount );
    virtual ~CTestModeCommand();

private:
    // ICommand interface
    virtual std::string getDescription() const override;
    virtual bool execute() override;

private:
    IDataPublisherService & mPublisher;
    int mSamplesCount;
};

#endif // CTESTMODECOMMAND_H