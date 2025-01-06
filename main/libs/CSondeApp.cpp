#include "CSondeApp.h"
#include "publisher/CBluetoothPublisherService.h"
#include "command/CCommandProcessor.h"

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

    //Create BT Service
    CBluetoothPublisherService publisher;
    IDataPublisherService & rPublisher = publisher;
    IRebootable & rRebootable = *this;

    // Create command processor
    CCommandProcessor processor( rPublisher, rRebootable );
    ICommandListener * pCmdListener = &processor;

    // Specify listener for incoming Bluetooth commands
    publisher.setNotificationListener( pCmdListener );

    // Start BT Service to wait for client commands
    rPublisher.start();

    // Enter command processing loop
    while( true )
    {
        processor.processCommands();

        if ( mRebootPending )
        {
            break;
        }
    }
}