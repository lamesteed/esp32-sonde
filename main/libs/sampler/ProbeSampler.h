
#ifndef PROBESAMPLER_H
#define PROBESAMPLER_H

#include "ISampler.h"
#include "CCalibrationConfigHelper.h"
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

    /// @brief Initialize sampler, must be called before first call to getSample()
    ///        After this call sampler considered ready to provide samples
    /// @return true if initialization successful, false otherwise
    virtual bool init( const CalibrationConfig & config ) override;

    /// @brief  Retrieve next sample from sensors
    /// @return Sample data, nullptr if sampling failed
    virtual  SampleData::Ptr getSample() override;

private:


    float getTemperatureInCelsius();
    float calculate_tds_from_voltage(float tds_input_voltage, float temperature, float factorA, float factorB);
    float getConductivity (float tds_input_voltage);
    void readAllSensors( SampleData & data );
    SampleData::Ptr averageSensorReadings(int numSamples);

    static const char * TAG;

    // Configuration parameter names
    static const char * CFG_NUMBER_OF_SAMPLES;
    static const char * CFG_TDS_CONVERSION_FACTOR_A;
    static const char * CFG_TDS_CONVERSION_FACTOR_B;
    static const char * CFG_PRESSURE_CONVERSION_FACTOR_A;
    static const char * CFG_PRESSURE_CONVERSION_FACTOR_B;
    static const char * CFG_PH_CONVERSION_FACTOR_A;
    static const char * CFG_PH_CONVERSION_FACTOR_B;
    static const char * CFG_DO_CONVERSION_FACTOR_A;
    static const char * CFG_DO_CONVERSION_FACTOR_B;
    static const char * CFG_FILENAME;

    CalibrationParams mCalibrationParameters;           ///< Expected calibration parameters and their default values
    CCalibrationConfigHelper::Ptr mConfigHelper;        ///< Calibration configuration helper

    std::shared_ptr<OneWire> mOneWirePtr;               ///< OneWire instance
    std::shared_ptr<DallasTemperature> mTempSensorPtr;  ///< Temperature sensor instance

    ITimeService::Ptr mTimeService;                     ///< Time service instance
};

#endif // PROBESAMPLER_H