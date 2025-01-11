#include "CCommandProcessor.h"
#include "IDataPublisherService.h"
#include "IRebootable.h"
#include "CTestModeCommand.h"
#include "CRebootCommand.h"
#include "esp_log.h"

// constants definition
const char * CCommandProcessor::TAG = "CCommandProcessor";
const char * CCommandProcessor::CMD_TESTMODE = "TESTMODE";
const char * CCommandProcessor::CMD_REBOOT = "REBOOT";

CCommandProcessor::CCommandProcessor(
    const ISampler::Ptr & sampler,
    const IDataPublisherService::Ptr & publisher,
    const IRebootable::Ptr & rebootable )
        : mSampler( sampler )
        , mPublisher( publisher )
        , mRebootable( rebootable )
        , mCommandQueue()
        , mQueueMutex()
        , mCondvar()
{
    ESP_LOGI( TAG, "Instance created" );
}

CCommandProcessor::~CCommandProcessor()
{
    ESP_LOGI( TAG, "Instance destroyed" );
}

void CCommandProcessor::onCommandReceived( const std::string & command, const std::string & args )
{
    ESP_LOGI( TAG, "onCommandReceived() - command: %s, args: %s", command.c_str(), args.c_str() );

    ICommand::Ptr cmd;

    if ( !command.compare( CMD_TESTMODE ) )
    {
        // create and execute test mode command
        static const int SAMPLES_COUNT = 5;
        cmd = std::make_shared<CTestModeCommand>( mSampler, mPublisher, SAMPLES_COUNT );
    } else if ( !command.compare( CMD_REBOOT ) )
    {
        // create and execute reboot command
        cmd = std::make_shared<CRebootCommand>( mPublisher, mRebootable );
    } else
    {
        ESP_LOGE( TAG, "onCommandReceived() - unknown command: %s", command.c_str() );
    }

    // if cmd is not null, add it to the queue
    if ( cmd != nullptr )
    {
        std::lock_guard<std::mutex> lock( mQueueMutex );
        mCommandQueue.push( cmd );
        mCondvar.notify_one();
    }
}

void CCommandProcessor::processCommands()
{
    // Lock the mutex andi wait until there is a command in the queue
    std::unique_lock<std::mutex> lock( mQueueMutex );
    mCondvar.wait( lock, [this]() { return !mCommandQueue.empty(); } );

    // Retrieve the command from queue
    auto cmd = mCommandQueue.front();
    mCommandQueue.pop();
    lock.unlock();

    // Print command description in the log
    ESP_LOGI( TAG, "Processing command: %s", cmd->getDescription().c_str() );

    // Execute the command
    bool result = cmd->execute();

    // Log the result
    ESP_LOGI( TAG, "Command executed, result: %s", result ? "success" : "failure" );
}
