
#include "CBluetoothPublisherService.h"

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
const char * CBluetoothPublisherService::CHAR_RX_UUID      = "e3223119-9445-4e96-a4a1-85358c4046a2";

CBluetoothPublisherService::CBluetoothPublisherService()
    : mServer( nullptr )
    , mNotifyCharacteristic( nullptr )
    , mRxCharacteristic( nullptr )
    , mNotifyDescriptor2901( new BLEDescriptor( ( uint16_t ) 0x2901 ) )
    , mNotifyDescriptor2902( new BLE2902() )
    , mRxDescriptor2901( new BLEDescriptor( ( uint16_t ) 0x2901 ) )
    , mRxDescriptor2902( new BLE2902() )
    , mDeviceConnected( false )
    , mNotificationsListenerReady( false )
{
    ESP_LOGI( TAG, "Instance created" );
    // configure descriptors
    mNotifyDescriptor2901->setValue( "Subscribe to get sonde data" );
    mNotifyDescriptor2902->setNotifications( true );
    mRxDescriptor2901->setValue( "Write to initiate sonde data transfer" );
}

CBluetoothPublisherService::~CBluetoothPublisherService()
{
    ESP_LOGI( TAG, "Instance destroyed" );
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
    mNotificationsListenerReady = false;
    mDeviceConnected = false;
}
// BLEServerCallbacks interface implementation end


// BLECharacteristicCallbacks interface implementation
void CBluetoothPublisherService::onWrite(BLECharacteristic *pChar)
{
    ESP_LOGI( TAG, "onWrite() - data received, enble notifications" );
    mNotificationsListenerReady = true;
}
// BLECharacteristicCallbacks interface implementation end


bool CBluetoothPublisherService::initBLEService()
{
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

    // Create Notify characteristic (used to send data to client)
    // Add 2901 and 2902 descriptors
    mNotifyCharacteristic =
        pService->createCharacteristic( CHAR_NOTIFY_UUID,
                                        BLECharacteristic::PROPERTY_NOTIFY );
    if ( !mNotifyCharacteristic )
    {
        ESP_LOGE( TAG, "BLE Notification characteristic creation failed" );
        return false;
    }

    // Create Read/Write characteristic (client uses it to initiate data transfer)
    // Add 2901 and 2902 descriptors
    // Also add callback to trigger data transfer
    mRxCharacteristic =
        pService->createCharacteristic( CHAR_RX_UUID,
                                        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE );
    if ( !mRxCharacteristic )
    {
        ESP_LOGE( TAG, "BLE Read/Write characteristic creation failed" );
        return false;
    }

    mNotifyCharacteristic->addDescriptor( mNotifyDescriptor2901.get() );
    mNotifyCharacteristic->addDescriptor( mNotifyDescriptor2902.get() );

    mRxCharacteristic->addDescriptor( mRxDescriptor2901.get() );
    mRxCharacteristic->addDescriptor( mRxDescriptor2902.get() );
    mRxCharacteristic->setCallbacks( this );

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

    BLEDevice::startAdvertising();

    return true;
}

bool CBluetoothPublisherService::publishData( const SampleDataList & data )
{
    ESP_LOGI( TAG, "publishData() - publishing data ..." );
    // start BLE service that has two characteristics
    // One is Read/Write will be used to initiate data transfer once client connects and writes to it
    // Second is Notify only, will be used to send data to client
    if ( !initBLEService() )
    {
        ESP_LOGE( TAG, "BLE initialization failed" );
        return false;
    }

    ESP_LOGI(TAG, "BLE Service started, waiting for connection ...");

    // concatenate all data samples into one string separated by new line
    // this will be sent to client in 20 bytes chunks
    // each chunk will be sent as a separate notification
    std::string allData;
    for( const auto & sample : data )
    {
        allData += sample + " ***\n ";
        
    }

    // loop and wait for client to connect and write to characteristic 2
    // send all data to client (20 bytes at a time) using characteristic 1
    while ( true )
    {
        if ( mDeviceConnected && mNotificationsListenerReady )
        {
            ESP_LOGI( TAG, "publishData() - device connected and ready to receive data" );
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

            break;
        }

        delayMsec( 1000 );
    }

    //once all data sent - return, this will conclude application cycle
    ESP_LOGI( TAG, "publishData() - data published" );
    return true;
}