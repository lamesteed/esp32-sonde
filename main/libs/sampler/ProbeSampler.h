
#ifndef PROBESAMPLER_H
#define PROBESAMPLER_H

#include "ISampler.h"
#include "CComponentConfigHelper.h"
#include "ITimeService.h"

class OneWire;
class DallasTemperature;

class ProbeSampler : public ISampler
{
public:
    /// @brief Constructor
    ProbeSampler(const ITimeService::Ptr & timeService);

    /// @brief Virtual destructor
    virtual ~ProbeSampler();

private:
    // ISampler interface

    /// @brief Initialize sampler
    /// @param config Configuration parameters to be used for sampling
    /// @return true if initialization successful, false otherwise
    virtual bool init( const ComponentConfig & config ) override;

    /// @brief  Retrieve next sample from sensors
    /// @return Sample data, nullptr if sampling failed
    virtual  SampleData::Ptr getSample() override;

private:


    float getTemperatureInCelsius();
    float calculate_tds_from_voltage(float tds_input_voltage, float temperature, float factorA, float factorB);
    float tds_temp_compensation(float tds_input_voltage, float temperature );
    void readAllSensors( SampleData & data );
    SampleData::Ptr averageSensorReadings(int numSamples);

    static const char * TAG;

    // Configuration parameter names
    static const char * CFG_NUMBER_OF_SAMPLES;
    static const char * CFG_TDS_CONVERSION_FACTOR_A;
    static const char * CFG_TDS_CONVERSION_FACTOR_B;
    static const char * CFG_TDS_CONVERSION_FACTOR_C;
    static const char * CFG_TDS_CONVERSION_FACTOR_D;
    static const char * CFG_PRESSURE_CONVERSION_FACTOR_A;
    static const char * CFG_PRESSURE_CONVERSION_FACTOR_B;
    static const char * CFG_PH_CONVERSION_FACTOR_A;
    static const char * CFG_PH_CONVERSION_FACTOR_B;
    static const char * CFG_DO_CONVERSION_FACTOR_A;
    static const char * CFG_DO_CONVERSION_FACTOR_B;
    static const char * CFG_FILENAME;

    ComponentConfigDefaults mDefaultCalibration;        ///< Expected calibration parameters and their default values
    CComponentConfigHelper::Ptr mConfigHelper;          ///< Configuration parameters helper

    std::shared_ptr<OneWire> mOneWirePtr;               ///< OneWire instance
    std::shared_ptr<DallasTemperature> mTempSensorPtr;  ///< Temperature sensor instance

    ITimeService::Ptr mTimeService;                     ///< Time service instance
};

#endif // PROBESAMPLER_H