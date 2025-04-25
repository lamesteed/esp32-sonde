#include "CSondeApp.h"
#include "CSystemTimeService.h"
#include "CBluetoothPublisherService.h"
#include "CCsvSerializer.h"
#include "CPlainTextSerializer.h"
#include "CStorageService.h"
#include "CMemoryStorageService.h"
#include "CCommandProcessor.h"
#include "esp_log.h"

#define USE_DUMMY_SAMPLER 0

#if USE_DUMMY_SAMPLER
#include "CDummySampler.h"
#else
#include "ProbeSampler.h"
#endif

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
#if USE_DUMMY_SAMPLER
    ESP_LOGI( TAG, "Using dummy sampler" );
    ISampler::Ptr sampler = std::make_shared<CDummySampler>( systime );
#else
    ESP_LOGI( TAG, "Using probe sampler" );
    ISampler::Ptr sampler = std::make_shared<ProbeSampler>( systime );
#endif

    // CREATE CSV serializer
    ISampleSerializer::Ptr serializer = std::make_shared<CCsvSerializer>( systime );
    ISampleSerializer::Ptr serializerTxt = std::make_shared<CPlainTextSerializer>();

    // Safely create shared pointer to this object
    IRebootable::Ptr rebootable = shared_from_this();


    // Create command processor
    CCommandProcessor processor( sampler, serializer, serializerTxt, publisher, rebootable, storage, systime );

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