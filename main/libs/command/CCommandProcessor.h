#ifndef CCOMMANDPROCESSOR_H
#define CCOMMANDPROCESSOR_H

#include "ICommandListener.h"
#include "IDataPublisherService.h"
#include "ICommand.h"

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

class CCommandProcessor : public ICommandListener
{
public:
    /// @brief Default constructor
    CCommandProcessor( IDataPublisherService & publisher );

    /// @brief Destructor
    virtual ~CCommandProcessor();

    void processCommands();

    // ICommandListener interface declaration
    virtual void onCommandReceived( const std::string & command, const std::string args ) override;

private:

private:
    static const char * TAG;                ///< Logger tag
    static const char * CMD_TESTMODE;       ///< Command name for test mode

    IDataPublisherService & mPublisher;

    std::queue<std::shared_ptr<ICommand>> mCommandQueue;
    std::mutex mQueueMutex;
    std::condition_variable mCondvar;
};

#endif // CCOMMANDPROCESSOR_H