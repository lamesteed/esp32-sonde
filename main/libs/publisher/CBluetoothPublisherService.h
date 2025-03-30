#ifndef CBLUETOOTHPUBLISHERSERVICE_H
#define CBLUETOOTHPUBLISHERSERVICE_H

#include "IDataPublisherService.h"
#include "BLEServer.h"
#include "BLECharacteristic.h"

class BLEDescriptor;
class BLE2902;

class CBluetoothPublisherService : public IDataPublisherService
                                 , public BLEServerCallbacks
                                 , public BLECharacteristicCallbacks
{
public:
    CBluetoothPublisherService();
    virtual ~CBluetoothPublisherService();

private:
    // IDataPublisherService interface
    virtual bool start() override;
    virtual bool stop() override;
    virtual bool publishData( const std::string & data, bool sendEOD ) override;
    virtual void setNotificationListener( ICommandListener * listener );

    // BLEServerCallbacks interface
    virtual void onConnect( BLEServer * pServer ) override;
    virtual void onDisconnect( BLEServer * pServer ) override;
    virtual void onMtuChanged(BLEServer *pServer, esp_ble_gatts_cb_param_t *param) override;

    // BLECharacteristicCallbacks interface
    virtual void onWrite( BLECharacteristic * pChar ) override;

private:
    /// @brief Initialize BLE service
    /// @return true if service initialized and started successfully, false otherwise
    bool initBLEService();

    static const char * TAG;                ///< Logger tag
    static const char * BLE_DEVICE_NAME;    ///< BLE device name
    static const char * SERVICE_UUID;       ///< UUID for service
    static const char * CHAR_NOTIFY_UUID;   ///< UUID for notification characteristic (is used to send command output to client)
    static const char * CHAR_RX_CMD_UUID;   ///< UUID for Read/Write command characteristic (client uses it to specify trigger command for execution)
    static const char * CHAR_RX_ARGS_UUID;  ///< UUID for Read/Write command arguments characteristic (client uses it to specify command arguments)

    ICommandListener * mNotificationListener;  ///< Listener for commands from BT client

    BLEServer         * mServer;                    ///< BLE server instance
    BLECharacteristic * mNotifyCharacteristic;      ///< Notify characteristic to send command output to client
    BLECharacteristic * mRxCmdCharacteristic;       ///< Read/Write characteristic to initiate command execution
    BLECharacteristic * mRxCmdArgsCharacteristic;   ///< Read/Write characteristic to

    std::shared_ptr<BLEDescriptor> mNotifyDescriptor2901; ///< BLE descriptor 2901 for Notify characteristic
    std::shared_ptr<BLE2902> mNotifyDescriptor2902;       ///< BLE descriptor 2902 for Notify characteristic
    std::shared_ptr<BLEDescriptor> mRxDescriptor2901;     ///< BLE descriptor 2901 for Read/Write command characteristic
    std::shared_ptr<BLE2902> mRxDescriptor2902;           ///< BLE descriptor 2902 for Read/Write command characteristic
    std::shared_ptr<BLEDescriptor> mRxArgsDescriptor2901; ///< BLE descriptor 2901 for Read/Write command arguments characteristic
    std::shared_ptr<BLE2902> mRxArgsDescriptor2902;       ///< BLE descriptor 2902 for Read/Write command arguments characteristic

    bool mDeviceConnected;                  ///< Flag indicating if device is connected
    std::string mCurrentCommand;            ///< Current command received from client
    std::string mCurrentCommandArgs;        ///< Current command arguments received from client

    size_t mMaxMtu;                        ///< Maximum MTU size
};

#endif // CBLUETOOTHPUBLISHERSERVICE_H