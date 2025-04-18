
#ifndef PROBESAMPLER_H
#define PROBESAMPLER_H

#include "ISampler.h"
#include "IStorageService.h"
#include "CCalibrationConfigHelper.h"

class OneWire;
class DallasTemperature;

class ProbeSampler : public ISampler
{
public:
    /// @brief Constructor
    ProbeSampler(const IStorageService::Ptr & storage);

    /// @brief Virtual destructor
    virtual ~ProbeSampler();

private:
    // ISampler interface

    /// @brief Initialize sampler, must be called before first call to getSample()
    ///        After this call sampler considered ready to provide samples
    /// @return true if initialization successful, false otherwise
    virtual bool init( const CalibrationConfig & config ) override;

    /// @brief  Retrieve next sample from sensors
    /// @return Sample data serialized to satring,
    ///         empty string if no more samples available in current cycle (surface reached)
    virtual  std::string getSample() override;

private:
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

        SampleData() : temperature(0), pressure_voltage(0), pressure(0),
                       tds_voltage(0), tds(0), conductivity(0), ph_voltage(0), ph(0), do2_voltage(0), do2(0)
        {
        }
    };

    float getTemperatureInCelsius();
    float getAnalogInputVoltage (int inputPin);
    float calculate(float input_voltage, const std::string &factorAKey, const std::string &factorBKey);
    float getTDS (float tds_input_voltage);
    float getPH (float ph_input_voltage);
    float getDO (float do_input_voltage);
    float getConductivity (float tds_input_voltage);
    float getPressure (float pressure_input_voltage);
    void readAllSensors( SampleData & data );
    std::string twoDecimalString(float value);
    std::string writeSampleDataInTestingMode (const SampleData::Ptr & data, int counter);
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

    IStorageService::Ptr mStorage;
    bool mTestMode; ///< Flag indicating if test mode is enabled
};

#endif // PROBESAMPLER_H