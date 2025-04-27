#include "ProbeSampler.h"
#include "Sensor.h"
#include "delay.h"
#include "esp_log.h"

#include <OneWire.h>
#include <DallasTemperature.h>

#define PRESSURE_SENSOR_INPUT_PIN "adc0"         // ADS1115 ADC pin A0 to pressure sensor 
#define TDS_SENSOR_INPUT_PIN      34        // pin GPIO34 (ADC1) to TDS sensor
#define TEMP_SENSOR_INPUT_PIN     18        // pin GPIO18 to DS18B20 sensor's DATA pin
#define PH_SENSOR_INPUT_PIN       26        // pin GPIO26 to PH sensor
#define DO_SENSOR_INPUT_PIN       35        // pin GPIO35 to DO sensor

Sensor analog_sensor;
const char * ProbeSampler::TAG = "ProbeSampler";

const char * ProbeSampler::CFG_NUMBER_OF_SAMPLES            = "NUMBER_OF_SAMPLES";
const char * ProbeSampler::CFG_TDS_CONVERSION_FACTOR_A      = "TDS_CONVERSION_FACTOR_A";
const char * ProbeSampler::CFG_TDS_CONVERSION_FACTOR_B      = "TDS_CONVERSION_FACTOR_B";
const char * ProbeSampler::CFG_PRESSURE_CONVERSION_FACTOR_A = "PRESSURE_CONVERSION_FACTOR_A";
const char * ProbeSampler::CFG_PRESSURE_CONVERSION_FACTOR_B = "PRESSURE_CONVERSION_FACTOR_B";
const char * ProbeSampler::CFG_PH_CONVERSION_FACTOR_A      = "PH_CONVERSION_FACTOR_A";
const char * ProbeSampler::CFG_PH_CONVERSION_FACTOR_B      = "PH_CONVERSION_FACTOR_B";
const char * ProbeSampler::CFG_DO_CONVERSION_FACTOR_A = "DO_CONVERSION_FACTOR_A";
const char * ProbeSampler::CFG_DO_CONVERSION_FACTOR_B = "DO_CONVERSION_FACTOR_B";
const char * ProbeSampler::CFG_FILENAME = "FILENAME";

ProbeSampler::ProbeSampler( const ITimeService::Ptr & timeService )
        : mCalibrationParameters( { { CFG_NUMBER_OF_SAMPLES, "10" },
                                    { CFG_TDS_CONVERSION_FACTOR_A, "1220" },
                                    { CFG_TDS_CONVERSION_FACTOR_B, "0" },
                                    { CFG_PRESSURE_CONVERSION_FACTOR_A, "25.13" },
                                    { CFG_PRESSURE_CONVERSION_FACTOR_B, "-13.07" },
                                    { CFG_PH_CONVERSION_FACTOR_A, "2.8" },
                                    { CFG_PH_CONVERSION_FACTOR_B, "0" },
                                    { CFG_DO_CONVERSION_FACTOR_A, "2381" },
                                    { CFG_DO_CONVERSION_FACTOR_B, "0" },
                                    { CFG_FILENAME, "output.csv" }} )
        , mConfigHelper()
        , mOneWirePtr( std::make_shared<OneWire>( TEMP_SENSOR_INPUT_PIN ) )
        , mTempSensorPtr( std::make_shared<DallasTemperature>( mOneWirePtr.get() ) )
        , mTimeService( timeService )
{
    ESP_LOGI( TAG, "Instance created" );
}

float ProbeSampler::getTemperatureInCelsius () {
    mTempSensorPtr->requestTemperatures();      // Request temperature readings
    return mTempSensorPtr->getTempCByIndex(0);  // Read temperature in °C
}

float ProbeSampler::getConductivity (float tds_input_voltage) {
    /* Conductivity = TDS * conversion factor, where
        0.65 - general purpose
        0.5  - sea water
        0.7  - drinking water
        0.8  - hydroponics */
    float conductivity = analog_sensor.getValue(tds_input_voltage, mConfigHelper->getAsFloat(CFG_TDS_CONVERSION_FACTOR_A), mConfigHelper->getAsFloat(CFG_TDS_CONVERSION_FACTOR_B))/0.7; // assuming 0.7 conversion factor
    return (conductivity > 0) ? conductivity : 0;
}

void clamp_result(float* result, int min_value, int max_value) {
    /* Clamp the result to be within the range [0, 100] */
    if (*result < min_value) {
        *result = min_value;
    } else if (*result > max_value) {
        *result = max_value;
    }
}
void ProbeSampler::readAllSensors( SampleData & data ) {
    ESP_LOGI( TAG, "Reading sensors..." );

    data.temperature = getTemperatureInCelsius();
    data.pressure_voltage = analog_sensor.getAnalogInputVoltage(PRESSURE_SENSOR_INPUT_PIN);
    data.pressure = analog_sensor.getValue(data.pressure_voltage, mConfigHelper->getAsFloat(CFG_PRESSURE_CONVERSION_FACTOR_A), mConfigHelper->getAsFloat(CFG_PRESSURE_CONVERSION_FACTOR_B));
    data.tds_voltage = analog_sensor.getAnalogInputVoltage(TDS_SENSOR_INPUT_PIN);
    data.tds = analog_sensor.getValue(data.tds_voltage, mConfigHelper->getAsFloat(CFG_TDS_CONVERSION_FACTOR_A), mConfigHelper->getAsFloat(CFG_TDS_CONVERSION_FACTOR_B));
    data.conductivity = getConductivity(data.tds_voltage);
    data.ph_voltage = analog_sensor.getAnalogInputVoltage(PH_SENSOR_INPUT_PIN);
    data.ph = analog_sensor.getValue(data.ph_voltage, mConfigHelper->getAsFloat(CFG_PH_CONVERSION_FACTOR_A), mConfigHelper->getAsFloat(CFG_PH_CONVERSION_FACTOR_B));
    clamp_result(&data.ph,0,14);
    data.do2_voltage = analog_sensor.getAnalogInputVoltage(DO_SENSOR_INPUT_PIN)/11; //11x gain as per sensor spec, plus calibration adjustment to get the right probe voltage
    data.do2 = analog_sensor.getValue(data.do2_voltage, mConfigHelper->getAsFloat(CFG_DO_CONVERSION_FACTOR_A), mConfigHelper->getAsFloat(CFG_DO_CONVERSION_FACTOR_B));
    clamp_result(&data.do2, 0, 100);


}

SampleData::Ptr ProbeSampler::getSample() {
    int numSamples = mConfigHelper->getAsInt( CFG_NUMBER_OF_SAMPLES );
    ESP_LOGI( TAG, "Sampling data, numSamples = %d", numSamples );

    SampleData::Ptr accumulatedData = std::make_shared<SampleData>();
    accumulatedData->tsStart = mTimeService->getTime();
    accumulatedData->temperature = -127; // initialize to minimal possible value

    std::vector<float> temp(numSamples);

    for (int i = 0; i < numSamples; ++i) {
        SampleData data;
        readAllSensors( data );
        temp[i] = data.temperature;
        // for temperature reading save the highest value
        if (data.temperature > accumulatedData->temperature )
        {
            accumulatedData->temperature = data.temperature;
        }
        accumulatedData->pressure += data.pressure;
        accumulatedData->pressure_voltage += data.pressure_voltage;
        accumulatedData->tds += data.tds;
        accumulatedData->tds_voltage += data.tds_voltage;
        accumulatedData->conductivity += data.conductivity;
        accumulatedData->ph_voltage += data.ph_voltage;
        accumulatedData->ph += data.ph;
        accumulatedData->do2_voltage += data.do2_voltage;
        accumulatedData->do2 += data.do2;
        delayMsec( 10 );                               // delay 10ms between each sample
    }

    accumulatedData->pressure /= numSamples;
    accumulatedData->pressure_voltage /= numSamples;
    accumulatedData->tds /= numSamples;
    accumulatedData->tds_voltage /= numSamples;
    accumulatedData->conductivity /= numSamples;
    accumulatedData->ph_voltage /= numSamples;
    accumulatedData->ph /= numSamples;
    accumulatedData->do2_voltage /= numSamples;
    accumulatedData->do2 /= numSamples;

    accumulatedData->tsEnd = mTimeService->getTime();
    accumulatedData->depth = accumulatedData->pressure * -0.703; // convert pressure (psi) to depth (m)

    return accumulatedData;
}

ProbeSampler::~ProbeSampler()
{
    ESP_LOGI( TAG, "Instance destroyed" );
}

bool ProbeSampler::init( const CalibrationConfig & config ) {
    ESP_LOGI( TAG, "Initializing ..." );
    mConfigHelper = std::make_shared<CCalibrationConfigHelper>( config, mCalibrationParameters );
    // Iterate through expected calibration parameters and print values that will be used for sampling
    ESP_LOGI( TAG, "Calibration parameters that will be applied:" );
    for ( auto & param : mCalibrationParameters )
    {
        ESP_LOGI( TAG, "%s = %s", param.first.c_str(), mConfigHelper->getAsString( param.first ).c_str() );
    }

    mTempSensorPtr->begin();                             // initialize temperature sensor
    delayMsec( 1000 );
    ESP_LOGI( TAG, "Initializing complete, ready to sample" );
    return true;
}
