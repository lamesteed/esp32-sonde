#ifndef CCOMMANDPROCESSOR_H
#define CCOMMANDPROCESSOR_H

#include "ICommandListener.h"
#include "ISampler.h"
#include "IDataPublisherService.h"
#include "IRebootable.h"
#include "IStorageService.h"
#include "ITimeService.h"
#include "ICommand.h"

#include <queue>
#include <mutex>
#include <condition_variable>

/// @brief Command processor - listens for incoming commands and processes them
class CCommandProcessor : public ICommandListener
{
public:
    /// @brief Default constructor
    /// @param sampler        Sampler instance
    /// @param publisher      Data publisher instance
    /// @param rebootable     Rebootable instance
    /// @param storageService Storage service instance
    /// @param timeService    Time service instance
    CCommandProcessor(
        const ISampler::Ptr & sampler,
        const IDataPublisherService::Ptr & publisher,
        const IRebootable::Ptr & rebootable,
        const IStorageService::Ptr & storageService,
        const ITimeService::Ptr & timeService );

    /// @brief Destructor
    virtual ~CCommandProcessor();

    /// @brief Process commands - executes next command in the queue (locks if queue is empty)
    void processCommands();

    /// @brief A command received from the client
    /// @param command Command name
    /// @param args    Command arguments ("key1=value1,key2=value2,..." format)
    virtual void onCommandReceived( const std::string & command, const std::string & args ) override;

private:

private:
    static const char * TAG;                ///< Logger tag
    static const char * CMD_TESTMODE;       ///< Command name for test mode
    static const char * CMD_REBOOT;         ///< Command name for reboot
    static const char * CMD_LISTFILES;      ///< Command name for list files
    static const char * CMD_GETFILE;        ///< Command name for get file
    static const char * CMD_STOREFILE;      ///< Command name for store file
    static const char * CMD_SETTIME;        ///< Command name for set time

    /// @brief Static helper to parse string arguments into key-value map
    /// @param args string argument of "k1=v1,k2=v2,..." format
    /// @return map of key-value pairs of arguments
    static ICommand::CommandArgs parseArgs( const std::string & args );

    ISampler::Ptr mSampler;                 ///< Sampler instance
    IDataPublisherService::Ptr mPublisher;  ///< Data publisher instance
    IRebootable::Ptr mRebootable;           ///< Rebootable instance
    IStorageService::Ptr mStorageService;   ///< Storage service instance
    ITimeService::Ptr mTimeService;         ///< Time service instance

    std::queue< ICommand::Ptr > mCommandQueue;  ///< Command queue
    std::mutex mQueueMutex;                     ///< Mutex for command queue
    std::condition_variable mCondvar;           ///< Condition variable for command queue notification
};

#endif // CCOMMANDPROCESSOR_H