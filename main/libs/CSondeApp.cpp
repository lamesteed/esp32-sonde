#include "CSondeApp.h"

#include "esp_log.h"

const char * CSondeApp::TAG = "CSondeApp";

CSondeApp::CSondeApp( ISampler & sampler,
                      IDataPublisherService & publisher )
    : mSampler( sampler )
    , mPublisher( publisher )
{
    ESP_LOGI( TAG, "Instance created" );
}

void CSondeApp::run()
{
    ESP_LOGI( TAG, "Running ..." );

    IDataPublisherService::SampleDataList data;

    // Initialize sampler
    if( !mSampler.init() )
    {
        ESP_LOGE( TAG, "Sampler initialization failed" );
        return;
    }

    // Retrieve samples
    while( true )
    {
        std::string sample = mSampler.getSample();
        if( sample.empty() )
        {
            ESP_LOGI( TAG, "No more samples available, publishing" );
            break;
        }

        data.push_back( sample );
    }

    // Publish collected data
    mPublisher.publishData( data );

    ESP_LOGI( TAG, "Data published, run() complete" );
}