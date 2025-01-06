#ifndef CCOMMANDPROCESSOR_H
#define CCOMMANDPROCESSOR_H

#include "ICommandListener.h"
#include "ICommand.h"

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

class IRebootable;
class IDataPublisherService;

class CCommandProcessor : public ICommandListener
{
public:
    /// @brief Default constructor
    CCommandProcessor( IDataPublisherService & publisher, IRebootable & rebootable );

    /// @brief Destructor
    virtual ~CCommandProcessor();

    void processCommands();

    // ICommandListener interface declaration
    virtual void onCommandReceived( const std::string & command, const std::string args ) override;

private:

private:
    static const char * TAG;                ///< Logger tag
    static const char * CMD_TESTMODE;       ///< Command name for test mode
    static const char * CMD_REBOOT;         ///< Command name for reboot

    IDataPublisherService & mPublisher;
    IRebootable & mRebootable;

    std::queue<std::shared_ptr<ICommand>> mCommandQueue;
    std::mutex mQueueMutex;
    std::condition_variable mCondvar;
};

#endif // CCOMMANDPROCESSOR_H