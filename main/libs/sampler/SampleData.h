#ifndef SAMPLE_DATA_H
#define SAMPLE_DATA_H

#include <memory>

struct SampleData {
    using Ptr = std::shared_ptr<SampleData>;

    float temperature;
    float pressure_voltage;
    float pressure;
    float tds_voltage;
    float tds;
    float conductivity;
    float ph_voltage;
    float ph;
    float do2_voltage;
    float do2;

    float depth; // in meters
    long long tsStart; // seconds since epoch
    long long tsEnd;   // seconds since epoch

    SampleData()
    : temperature( 0 )
    , pressure_voltage( 0 )
    , pressure( 0 )
    , tds_voltage( 0 )
    , tds( 0 )
    , conductivity( 0 )
    , ph_voltage( 0 )
    , ph( 0 )
    , do2_voltage( 0 )
    , do2(0)
    , depth( 0 )
    , tsStart(0)
    , tsEnd(0)
    {
    }
};

#endif // SAMPLE_DATA_H