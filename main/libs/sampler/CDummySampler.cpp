#include "CDummySampler.h"
#include "esp_log.h"
#include "delay.h"

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


const char * CDummySampler::TAG = "CDummySampler";

CDummySampler::CDummySampler( const int samples )
    : mSampleCounter( samples )
{
    ESP_LOGI( TAG, "Instance created (samples = %d)", samples );
}

CDummySampler::~CDummySampler()
{
    ESP_LOGI( TAG, "Instance destroyed" );
}

bool CDummySampler::init()
{
    ESP_LOGI( TAG, "Initializing ..." );

    sensors.begin();

    delayMsec( 1000 );
    ESP_LOGI( TAG, "Initializing complete, ready to sample" );
    return true;
}

std::string CDummySampler::getSample()
{
    delayMsec( 1000 );
    static int counter = 1;
    if( counter > mSampleCounter )
    {
        ESP_LOGI( TAG, "getSample() - no more samples" );
        return "";
    }
    else
    {
        ESP_LOGI( TAG, "getSample() retrived sample #%d ", counter );
        sensors.requestTemperatures();  // Request temperature readings

        std::string sampleData = "A Dummy Sample data " + std::to_string( counter++ );
        return sampleData;
    }
}