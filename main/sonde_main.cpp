#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"

#include "delay.h"

// Logger
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

// Arduino framework
#include "Arduino.h"

#include "CDummySampler.h"
#include "CBluetoothPublisherService.h"
#include "CSondeApp.h"

// Tag for logging
static const char * TAG = "sonde_main";

extern "C" void app_main(void)
{
    // Initialize the Arduino framework
    initArduino();

    // Create sampler and publisher
    CDummySampler sampler( 5 );
    CBluetoothPublisherService publisher;

    //Create application and run it
    CSondeApp app( sampler, publisher );
    app.run();

    // Restart the device
    for (int i = 10; i >= 0; i--) {
        ESP_LOGW(TAG, "Restarting in %d seconds...", i);
        delayMsec(1000);
    }
    ESP_LOGI(TAG, "Restarting now.");
    fflush(stdout);
    esp_restart();
}
