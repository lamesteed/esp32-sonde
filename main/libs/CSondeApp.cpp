#include "CSondeApp.h"
#include "publisher/CBluetoothPublisherService.h"
#include "command/CCommandProcessor.h"

#include "esp_log.h"

const char * CSondeApp::TAG = "CSondeApp";

CSondeApp::CSondeApp()
{
    ESP_LOGI( TAG, "Instance created" );
}

void CSondeApp::run()
{
    ESP_LOGI( TAG, "Running ..." );

    //Create BT Service
    CBluetoothPublisherService publisher;
    IDataPublisherService & rPublisher = publisher;

    // Create command processor
    CCommandProcessor processor( rPublisher );
    ICommandListener * pCmdListener = &processor;

    // Specify listener for incoming Bluetooth commands
    publisher.setNotificationListener( pCmdListener );

    // Start BT Service to wait for client commands
    rPublisher.start();

    // Enter command processing loop
    while( true )
    {
        processor.processCommands();
    }
}