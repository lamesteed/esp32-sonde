
#ifndef PROBESAMPLER_H
#define PROBESAMPLER_H

#include "ISampler.h"
#include "CCalibrationConfigHelper.h"

class OneWire;
class DallasTemperature;

class ProbeSampler : public ISampler
{
public:
    /// @brief Constructor
    ProbeSampler();

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
        float temperature;
        float pressure_voltage;
        float pressure;
        float tds_voltage;
        float tds;
        float conductivity;
    };

    struct DatasetFields {
        std::string DatasetName;
        std::string MonitoringLocationID;
        std::string MonitoringLocationName;
        std::string MonitoringLocationLatitude;
        std::string MonitoringLocationLongitude;
        std::string MonitoringLocationHorizontalCoordinateReferenceSystem;
        std::string MonitoringLocationHorizontalAccuracyMeasure;
        std::string MonitoringLocationHorizontalAccuracyUnit;
        std::string MonitoringLocationType;
        std::string ActivityType;
        std::string ActivityMediaName;
        std::string ActivityStartDate;
        std::string ActivityStartTime;
        std::string ActivityEndDate;
        std::string ActivityEndTime;
        std::string ActivityDepthHeightMeasure;
        std::string ActivityDepthHeightUnit;
        std::string SampleCollectionEquipmentName;
        std::string CharacteristicName;
        std::string MethodSpeciation;
        std::string ResultSampleFraction;
        std::string ResultValue;
        std::string ResultUnit;
        std::string ResultValueType;
        std::string ResultDetectionCondition;
        std::string ResultDetectionQuantitationLimitMeasure;
        std::string ResultDetectionQuantitationLimitUnit;
        std::string ResultDetectionQuantitationLimitType;
        std::string ResultStatusID;
        std::string ResultComment;
        std::string ResultAnalyticalMethodID;
        std::string ResultAnalyticalMethodContext;
        std::string ResultAnalyticalMethodName;
        std::string AnalysisStartDate;
        std::string AnalysisStartTime;
        std::string AnalysisEndTimeZone;
        std::string LaboratoryName;
        std::string LaboratorySampleID;
    };

    float getTemperatureInCelsius();
    float getAnalogInputVoltage (int inputPin);
    float getTDS (float tds_input_voltage, float temperature);
    float getConductivity (float tds_input_voltage, float temperature);
    float getPressure (float pressure_input_voltage);
    SampleData readAllSensors();
    std::string twoDecimalString(float value);
    std::string writeSampleDataInTestingMode (SampleData data, int counter);
    SampleData averageSensorReadings(int numSamples);

    static const char * TAG;

    // Configuration parameter names
    static const char * CFG_NUMBER_OF_SAMPLES;
    static const char * CFG_TDS_CONVERSION_FACTOR_A;
    static const char * CFG_TDS_CONVERSION_FACTOR_B;
    static const char * CFG_PRESSURE_CONVERSION_FACTOR_A;
    static const char * CFG_PRESSURE_CONVERSION_FACTOR_B;

    CalibrationParams mCalibrationParameters;           ///< Expected calibration parameters and their default values
    CCalibrationConfigHelper::Ptr mConfigHelper;        ///< Calibration configuration helper

    std::shared_ptr<OneWire> mOneWirePtr;               ///< OneWire instance
    std::shared_ptr<DallasTemperature> mTempSensorPtr;  ///< Temperature sensor instance
};

#endif // PROBESAMPLER_H