#ifndef CTESTMODECOMMAND_H
#define CTESTMODECOMMAND_H

#include "ICommand.h"
#include "ISampler.h"
#include "IDataPublisherService.h"

class CTestModeCommand : public ICommand
{
public:
    CTestModeCommand(
        const ISampler::Ptr & sampler,
        const IDataPublisherService::Ptr & publisher, int samplesCount );
    virtual ~CTestModeCommand() = default;

private:
    // ICommand interface
    virtual std::string getDescription() const override;
    virtual bool execute() override;

private:
    ISampler::Ptr mSampler;
    IDataPublisherService::Ptr mPublisher;
    int mSamplesCount;
};

#endif // CTESTMODECOMMAND_H