#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"

#include "delay.h"

// Logger
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

// Arduino framework
#include "Arduino.h"

#include "ProbeSampler.h"
#include "CSondeApp.h"

// Tag for logging
static const char * TAG = "sonde_main";

extern "C" void app_main(void)
{
    // Initialize the Arduino framework
    initArduino();

    //Create application and run it
    CSondeApp().run();

    // Restart the device
    for (int i = 5; i >= 0; i--) {
        ESP_LOGW( TAG, "Restarting in %d seconds...", i) ;
        delayMsec( 1000 );
    }
    ESP_LOGI( TAG, "Restarting now." );
    fflush( stdout );

    esp_restart();
}
