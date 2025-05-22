#include "CPlainTextSerializer.h"
#include "esp_log.h"
#include <sstream>

const char * CPlainTextSerializer::TAG = "CPlainTextSerializer";

CPlainTextSerializer::CPlainTextSerializer()
{
    ESP_LOGI( TAG, "Instance created" );
}

void CPlainTextSerializer::reset( const ComponentConfig & metadata )
{
    ESP_LOGI( TAG, "reset()" );
}

bool CPlainTextSerializer::serialize( const SampleData::Ptr & data, std::ostringstream & oss )
{
    ESP_LOGI( TAG, "serialize()" );

    if ( !data )
    {
        ESP_LOGE( TAG, "serialize() - data is null" );
        return false;
    }

    oss << "Temperature: " << data->temperature << std::endl
    << "Pressure: " << data->pressure << std::endl
    << "Pressure voltage: " << data->pressure_voltage << std::endl
    << "Depth: " << data->depth << std::endl
    << "TDS: " << data->tds << std::endl
    << "TDS voltage: " << data->tds_voltage << std::endl
    << "Conductivity: " << data->conductivity << std::endl
    << "pH: " << data->ph << std::endl
    << "pH voltage: " << data->ph_voltage << std::endl
    << "DO2: " << data->do2 << std::endl
    << "DO2 voltage: " << data->do2_voltage << std::endl
    << "================================" << std::endl;
    return true;
}
