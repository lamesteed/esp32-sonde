
#include "CBluetoothPublisherService.h"

#include "ICommandListener.h"
#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLE2902.h"

#include "delay.h"
#include "esp_log.h"

// constants definition
const char * CBluetoothPublisherService::TAG               = "CBluetoothPublisherService";
const char * CBluetoothPublisherService::BLE_DEVICE_NAME   = "ESP32-Sonde";
const char * CBluetoothPublisherService::SERVICE_UUID      = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const char * CBluetoothPublisherService::CHAR_NOTIFY_UUID  = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
const char * CBluetoothPublisherService::CHAR_RX_CMD_UUID  = "e3223119-9445-4e96-a4a1-85358c4046a2";
const char * CBluetoothPublisherService::CHAR_RX_ARGS_UUID = "8e0a2686-d5bc-11ef-9cd2-0242ac120002";

CBluetoothPublisherService::CBluetoothPublisherService()
    : mNotificationListener( nullptr )
    , mServer( nullptr )
    , mNotifyCharacteristic( nullptr )
    , mRxCmdCharacteristic( nullptr )
    , mRxCmdArgsCharacteristic( nullptr )
    , mNotifyDescriptor2901( new BLEDescriptor( ( uint16_t ) 0x2901 ) )
    , mNotifyDescriptor2902( new BLE2902() )
    , mRxDescriptor2901( new BLEDescriptor( ( uint16_t ) 0x2901 ) )
    , mRxDescriptor2902( new BLE2902() )
    , mRxArgsDescriptor2901( new BLEDescriptor( ( uint16_t ) 0x2901 ) )
    , mRxArgsDescriptor2902( new BLE2902() )
    , mDeviceConnected( false )
    , mCurrentCommand()
    , mCurrentCommandArgs()
{
    ESP_LOGI( TAG, "Instance created" );
    // configure descriptors
    mNotifyDescriptor2901->setValue( "Subscribe to get sonde command output" );
    mNotifyDescriptor2902->setNotifications( true );
    mRxDescriptor2901->setValue( "Write to initiate sonde command execution" );
    mRxArgsDescriptor2901->setValue( "Write to specify sonde command arguments" );
}

CBluetoothPublisherService::~CBluetoothPublisherService()
{
    stop();
    ESP_LOGI( TAG, "Instance destroyed" );
}

void CBluetoothPublisherService::setNotificationListener( ICommandListener * pListener )
{
    mNotificationListener = pListener;
}

// BLEServerCallbacks interface implementation
void CBluetoothPublisherService::onConnect(BLEServer* pServer)
{
    ESP_LOGW(TAG, "Device connected");
    mDeviceConnected = true;
}
void CBluetoothPublisherService::onDisconnect(BLEServer* pServer)
{
    ESP_LOGW(TAG, "Device disconnected");
    mCurrentCommand = "";
    mDeviceConnected = false;
    // Start service advertising again as advertising automatically stops when device is connected
    BLEDevice::startAdvertising();
}
// BLEServerCallbacks interface implementation end


// BLECharacteristicCallbacks interface implementation
void CBluetoothPublisherService::onWrite(BLECharacteristic *pChar)
{
    ESP_LOGI( TAG, "onWrite() - data received, characteristic = %s", pChar->getUUID().toString().c_str() );

    // if received argument chunk - append it to arguments string
    if ( pChar->getUUID().toString().equals( CHAR_RX_ARGS_UUID ) )
    {
        mCurrentCommandArgs.append( pChar->getValue().c_str() );
        ESP_LOGI( TAG, "onWrite() - Received argument chunk: %s", mCurrentCommandArgs.c_str() );
        return;
    }

    // if received command - assign to command string
    // then notify listener with current command and arguments and clear arguments to prepare for next command
    if ( pChar->getUUID().toString().equals( CHAR_RX_CMD_UUID ) )
    {
        mCurrentCommand.assign( pChar->getValue().c_str() );
        ESP_LOGI( TAG, "onWrite() - Received command: %s", mCurrentCommand.c_str() );

        // Notify listener about new command
        if ( mNotificationListener )
        {
            mNotificationListener->onCommandReceived( mCurrentCommand, mCurrentCommandArgs );
        }
        else
        {
            ESP_LOGE( TAG, "onWrite() - no listener to notify" );
        }

        mCurrentCommandArgs.clear();
        return;
    }
}
// BLECharacteristicCallbacks interface implementation end

bool CBluetoothPublisherService::start()
{
    if ( BLEDevice::getInitialized() )
    {
        ESP_LOGI( TAG, "BLE already initialized" );
        return true;
    }

    // Create the BLE Device
    BLEDevice::init( "" );

    // Create the BLE Server
    mServer = BLEDevice::createServer();
    if ( !mServer )
    {
        ESP_LOGE( TAG, "BLE server creation failed" );
        return false;
    }
    mServer->setCallbacks( this ); // see onConnect/onDisconnect callbacks above

    // Create the BLE Service
    BLEService * pService = mServer->createService( SERVICE_UUID );
    if ( !pService )
    {
        ESP_LOGE( TAG, "BLE service creation failed" );
        return false;
    }

    // Create Notify characteristic (is used to send command output to client)
    // Add 2901 and 2902 descriptors
    mNotifyCharacteristic =
        pService->createCharacteristic( CHAR_NOTIFY_UUID,
                                        BLECharacteristic::PROPERTY_NOTIFY );
    if ( !mNotifyCharacteristic )
    {
        ESP_LOGE( TAG, "BLE Notification characteristic creation failed" );
        return false;
    }
    mNotifyCharacteristic->addDescriptor( mNotifyDescriptor2901.get() );
    mNotifyCharacteristic->addDescriptor( mNotifyDescriptor2902.get() );

    // Create Read/Write characteristic (client uses it to specify command and trigger execution)
    // Add 2901 and 2902 descriptors
    // Also add callback to listen for incoming command from client
    mRxCmdCharacteristic =
        pService->createCharacteristic( CHAR_RX_CMD_UUID,
                                        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE );
    if ( !mRxCmdCharacteristic )
    {
        ESP_LOGE( TAG, "BLE Read/Write command characteristic creation failed" );
        return false;
    }
    mRxCmdCharacteristic->addDescriptor( mRxDescriptor2901.get() );
    mRxCmdCharacteristic->addDescriptor( mRxDescriptor2902.get() );
    mRxCmdCharacteristic->setCallbacks( this );

    // Create Read/Write characteristic (client uses it to specify command arguments)
    // Add 2901 and 2902 descriptors
    // Also add callback to listen for incoming command arguments from client
    mRxCmdArgsCharacteristic =
        pService->createCharacteristic( CHAR_RX_ARGS_UUID,
                                        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE );
    if ( !mRxCmdArgsCharacteristic )
    {
        ESP_LOGE( TAG, "BLE Read/Write command arguments characteristic creation failed" );
        return false;
    }
    mRxCmdArgsCharacteristic->addDescriptor( mRxArgsDescriptor2901.get() );
    mRxCmdArgsCharacteristic->addDescriptor( mRxArgsDescriptor2902.get() );
    mRxCmdArgsCharacteristic->setCallbacks( this );

    // Start the BLE service
    pService->start();

    // Start advertising
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID( SERVICE_UUID );
    pAdvertising->setScanResponse( false );
    pAdvertising->setMinPreferred( 0x0 );

    // Note we only need to define scan response if we have a BLE Service name that is too long
    BLEAdvertisementData advertisementData;
    BLEAdvertisementData scanResponseData;

    // Add UUIDs or other data to advertisement packet here if needed
    advertisementData.setFlags( ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT );
    pAdvertising->setAdvertisementData( advertisementData );

    // Add the device name to the scan response
    scanResponseData.setName( BLE_DEVICE_NAME );
    pAdvertising->setScanResponseData(scanResponseData);

    pAdvertising->start();

    return true;
}

bool CBluetoothPublisherService::stop()
{
    ESP_LOGI( TAG, "stop() - stopping BLE service" );

    if ( !BLEDevice::getInitialized() )
    {
        ESP_LOGI( TAG, "BLE not initialized" );
        return true;
    }

    // stop advertising
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    if( pAdvertising )
    {
        pAdvertising->stop();
        ESP_LOGI( TAG, "Advertising stopped" );
    }

    // stop service
    if ( mServer )
    {
        BLEService *pService = mServer->getServiceByUUID( SERVICE_UUID );
        if ( pService )
        {
            pService->stop();
            ESP_LOGI( TAG, "Service stopped" );
        }
    }

    // deinit BLE
    BLEDevice::deinit( false );
    mServer = nullptr;
    mNotifyCharacteristic = nullptr;
    mRxCmdCharacteristic = nullptr;
    mRxCmdArgsCharacteristic = nullptr;

    ESP_LOGI( TAG, "BLE stopped" );
    return true;
}

bool CBluetoothPublisherService::publishData( const std::string & data, bool sendEOD )
{
    ESP_LOGI( TAG, "publishData() - publishing data ..." );
    if ( !mDeviceConnected || mCurrentCommand.empty() )
    {
        ESP_LOGE( TAG, "publishData() - device not connected or not ready to receive data" );
        return false;
    }

    std::string allData = data;
    // send data in 20 bytes chunks (each chunk is a separate notification)
    while ( !allData.empty() )
    {
        std::string chunk = allData.substr( 0, 20 );
        allData.erase( 0, 20 );
        mNotifyCharacteristic->setValue( String( chunk.c_str(), chunk.size() ) );
        mNotifyCharacteristic->notify();
        ESP_LOGI( TAG, "publishData() - data sent: %s (%d bytes)", chunk.c_str(), chunk.size() );
        delayMsec( 500 );
    }

    if ( sendEOD )
    {
        static const String EOD = "END_OF_DATA";
        // send EOD (End Of Data) notification
        mNotifyCharacteristic->setValue( EOD );
        mNotifyCharacteristic->notify();
        ESP_LOGI( TAG, "publishData() - EOD sent" );
    }

    //once all data sent - return, this will conclude application cycle
    ESP_LOGI( TAG, "publishData() - data published" );
    return true;
}