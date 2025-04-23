#include "CCommandProcessor.h"
#include "IDataPublisherService.h"
#include "IRebootable.h"
#include "CTestModeCommand.h"
#include "CRebootCommand.h"
#include "CListFilesCommand.h"
#include "CGetFileCommand.h"
#include "CStoreFileCommand.h"
#include "CSetTimeCommand.h"
#include "esp_log.h"

// constants definition
const char * CCommandProcessor::TAG = "CCommandProcessor";
const char * CCommandProcessor::CMD_TESTMODE = "TESTMODE";
const char * CCommandProcessor::CMD_REBOOT = "REBOOT";
const char * CCommandProcessor::CMD_LISTFILES = "LISTFILES";
const char * CCommandProcessor::CMD_GETFILE = "GETFILE";
const char * CCommandProcessor::CMD_STOREFILE = "STOREFILE";
const char * CCommandProcessor::CMD_SETTIME = "SETTIME";
const char * CCommandProcessor::CMD_FIELDMODE = "FIELDMODE";

CCommandProcessor::CCommandProcessor(
    const ISampler::Ptr & sampler,
    const ISampleSerializer::Ptr & serializer,
    const IDataPublisherService::Ptr & publisher,
    const IRebootable::Ptr & rebootable,
    const IStorageService::Ptr & storageService,
    const ITimeService::Ptr & timeService )
        : mSampler( sampler )
        , mSerializer( serializer )
        , mPublisher( publisher )
        , mRebootable( rebootable )
        , mStorageService( storageService )
        , mTimeService( timeService )
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
        cmd = std::make_shared<CTestModeCommand>( mSampler, mSerializer, mStorageService, mPublisher, SAMPLES_COUNT );
    } else if ( !command.compare( CMD_REBOOT ) )
    {
        // create and execute reboot command
        cmd = std::make_shared<CRebootCommand>( mPublisher, mRebootable );
    } else if ( !command.compare( CMD_LISTFILES ) )
    {
        // create and execute list files command
        cmd = std::make_shared<CListFilesCommand>( mStorageService, mPublisher );
    } else if ( !command.compare( CMD_GETFILE ) )
    {
        //expect arguments for this command
        ICommand::CommandArgs cmdArgs = CCommandProcessor::parseArgs( args );
        // create and execute get file command
        cmd = std::make_shared<CGetFileCommand>( mStorageService, mPublisher, cmdArgs );
    } else if ( !command.compare( CMD_STOREFILE ) )
    {
        //expect arguments for this command
        ICommand::CommandArgs cmdArgs = CCommandProcessor::parseArgs( args );
        // create and execute store file command
        cmd = std::make_shared<CStoreFileCommand>( mStorageService, mPublisher, cmdArgs );
    } else if ( !command.compare( CMD_SETTIME ) )
    {
        //expect arguments for this command
        ICommand::CommandArgs cmdArgs = CCommandProcessor::parseArgs( args );
        // create and execute set time command
        cmd = std::make_shared<CSetTimeCommand>( mPublisher, mTimeService, cmdArgs );
    }
    /* else if ( !command.compare( CMD_FIELDMODE ) )
    {
        // create and execute field mode command
        cmd = std::make_shared<CFieldModeCommand>( mPublisher, mSampler );
    }*/
    else
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

ICommand::CommandArgs CCommandProcessor::parseArgs( const std::string & args )
{
    // Parsing args string argument of "k1=v1,k2=v2,..."" format into ICommand::CommandArgs map
    ICommand::CommandArgs cmdArgs;
    size_t pos = 0;
    while ( pos < args.size() )
    {
        size_t eqPos = args.find( '=', pos );
        if ( eqPos == std::string::npos )
        {
            ESP_LOGE( TAG, "parseArgs() - invalid args format: %s", args.c_str() );
            return cmdArgs;
        }

        std::string key = args.substr( pos, eqPos - pos );
        pos = eqPos + 1;

        size_t commaPos = args.find( ',', pos );
        std::string value = ( commaPos == std::string::npos ) ? args.substr( pos ) : args.substr( pos, commaPos - pos );
        pos = ( commaPos == std::string::npos ) ? args.size() : commaPos + 1;

        cmdArgs[key] = value;
    }

    return cmdArgs;
}