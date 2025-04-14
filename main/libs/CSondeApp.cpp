#include "CSondeApp.h"
#include "CSystemTimeService.h"
#include "CBluetoothPublisherService.h"
#include "ProbeSampler.h"
#include "CStorageService.h"
#include "CMemoryStorageService.h"
#include "CCommandProcessor.h"

#include "esp_log.h"

const char * CSondeApp::TAG = "CSondeApp";

CSondeApp::CSondeApp()
    : mRebootPending( false )
{
    ESP_LOGI( TAG, "Instance created" );
}

void CSondeApp::reboot()
{
    ESP_LOGI( TAG, "Device reboot requested" );
    mRebootPending = true;
}

void CSondeApp::run()
{
    ESP_LOGI( TAG, "Running ..." );

    // Create Time Service
    ITimeService::Ptr systime = std::make_shared<CSystemTimeService>();

    // Create BT Service
    IDataPublisherService::Ptr publisher = std::make_shared<CBluetoothPublisherService>();

    //Create storage service with memory decorator on top and start it
    IStorageService::Ptr storage =
        std::make_shared<CMemoryStorageService>( std::make_shared<CStorageService>() );
    bool storageStarted = storage->start();
    if ( !storageStarted )
    {
        ESP_LOGW( TAG, "Failed to start storage service, persisting data will not be possible" );
    }

    // Create sampler instance
    ISampler::Ptr sampler = std::make_shared<ProbeSampler>(storage);

    // Safely create shared pointer to this object
    IRebootable::Ptr rebootable = shared_from_this();


    // Create command processor
    CCommandProcessor processor( sampler, publisher, rebootable, storage, systime );

    // Specify listener for incoming Bluetooth commands
    publisher->setNotificationListener( &processor );

    // Start BT Service to wait for client commands
    publisher->start();

    // Enter command processing loop
    while( true )
    {
        processor.processCommands();

        if ( mRebootPending )
        {
            break;
        }
    }

    // Stop BT Service
    publisher->stop();

    // Stop storage service
    storage->stop();

    ESP_LOGI( TAG, "Stopped" );
}