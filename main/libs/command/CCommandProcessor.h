#ifndef CCOMMANDPROCESSOR_H
#define CCOMMANDPROCESSOR_H

#include "ICommandListener.h"
#include "ISampler.h"
#include "IDataPublisherService.h"
#include "IRebootable.h"
#include "ICommand.h"

#include <queue>
#include <mutex>
#include <condition_variable>

class CCommandProcessor : public ICommandListener
{
public:
    /// @brief Default constructor
    CCommandProcessor(
        const ISampler::Ptr & sampler,
        const IDataPublisherService::Ptr & publisher,
        const IRebootable::Ptr & rebootable );

    /// @brief Destructor
    virtual ~CCommandProcessor();

    void processCommands();

    // ICommandListener interface declaration
    virtual void onCommandReceived( const std::string & command, const std::string & args ) override;

private:

private:
    static const char * TAG;                ///< Logger tag
    static const char * CMD_TESTMODE;       ///< Command name for test mode
    static const char * CMD_REBOOT;         ///< Command name for reboot

    ISampler::Ptr mSampler;
    IDataPublisherService::Ptr mPublisher;
    IRebootable::Ptr mRebootable;

    std::queue< ICommand::Ptr > mCommandQueue;
    std::mutex mQueueMutex;
    std::condition_variable mCondvar;
};

#endif // CCOMMANDPROCESSOR_H