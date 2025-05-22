#include "ProbeSampler.h"
#include "Sensor.h"
#include "delay.h"
#include "esp_log.h"

#include <OneWire.h>
#include <DallasTemperature.h>

#define PRESSURE_SENSOR_INPUT_PIN "adc0"         // ADS1115 ADC pin A0 to pressure sensor
#define TDS_SENSOR_INPUT_PIN      "adc3"         // ADS1115 ADC pin A2 to pressure sensor
#define TEMP_SENSOR_INPUT_PIN     18             // pin GPIO18 to DS18B20 sensor's DATA pin
#define PH_SENSOR_INPUT_PIN       36             // pin GPIO36 to PH sensor
#define DO_SENSOR_INPUT_PIN       35             // pin GPIO35 to DO sensor
#define DO_INIT_TIMEOUT           5000          // timeout for DO sensor initialization
#define TARGET_DO_VOLTAGE         42e-3          // Target voltage of DO sensor in volts (42mV)
#define DO_STABLE_VOLTAGE_COUNT   5              // Number of consecutive stable readings required

Sensor analog_sensor;
Sensor do_sensor;

bool do_sensor_initialized = false;
const char * ProbeSampler::TAG = "ProbeSampler";

/* Conductivity = TDS * conversion factor, where
        0.65 - general purpose
        0.5  - sea water
        0.7  - drinking water
        0.8  - hydroponics */
const float tds_to_conductivity = 0.7;

const float pressure_to_depth = -0.703; // convert pressure (psi) to depth (m)

float do_calibration_voltage = 1;       //initialize variable to store DO sensor calibration voltage

const float do_air_max_v = 58e-3;   // DO sensor max voltage in air
const float do_air_min_v = 42e-3;       // DO sensor min voltage in air


const char * ProbeSampler::CFG_NUMBER_OF_SAMPLES            = "NUMBER_OF_SAMPLES";
const char * ProbeSampler::CFG_TDS_CONVERSION_FACTOR_A      = "TDS_CONVERSION_FACTOR_A";
const char * ProbeSampler::CFG_TDS_CONVERSION_FACTOR_B      = "TDS_CONVERSION_FACTOR_B";
const char * ProbeSampler::CFG_TDS_CONVERSION_FACTOR_C      = "TDS_CONVERSION_FACTOR_C";
const char * ProbeSampler::CFG_TDS_CONVERSION_FACTOR_D      = "TDS_CONVERSION_FACTOR_D";
const char * ProbeSampler::CFG_PRESSURE_CONVERSION_FACTOR_A = "PRESSURE_CONVERSION_FACTOR_A";
const char * ProbeSampler::CFG_PRESSURE_CONVERSION_FACTOR_B = "PRESSURE_CONVERSION_FACTOR_B";
const char * ProbeSampler::CFG_PH_CONVERSION_FACTOR_A      = "PH_CONVERSION_FACTOR_A";
const char * ProbeSampler::CFG_PH_CONVERSION_FACTOR_B      = "PH_CONVERSION_FACTOR_B";
const char * ProbeSampler::CFG_DO_CONVERSION_FACTOR_A = "DO_CONVERSION_FACTOR_A";
const char * ProbeSampler::CFG_DO_CONVERSION_FACTOR_B = "DO_CONVERSION_FACTOR_B";
const char * ProbeSampler::CFG_FILENAME = "FILENAME";

bool initialize_do_sensor(Sensor &do_sensor) {
    const char *TAG = "SensorInit";
    int stableReadings = 0;
    unsigned long startTime = millis();

    while (millis() - startTime < DO_INIT_TIMEOUT) {
        // Read the sensor voltage
        float voltage = analog_sensor.getAnalogInputVoltage(DO_SENSOR_INPUT_PIN)/11;

        // Check if the voltage is within the acceptable range
        if (voltage >= (TARGET_DO_VOLTAGE - 1e-3) && voltage <= (TARGET_DO_VOLTAGE + 1e-3)) {
            stableReadings++;
            ESP_LOGI(TAG, "Stable DO sensor reading %d: Voltage = %.3fV", stableReadings, voltage);

            // Exit the loop if the required number of stable readings is achieved
            if (stableReadings >= DO_STABLE_VOLTAGE_COUNT) {
                ESP_LOGI(TAG, "DO Sensor initialized successfully. Voltage stabilized at %.3fV.", voltage);
                return true;
            }
        } else {
            // Reset the counter if the reading is unstable
            stableReadings = 0;
            ESP_LOGW(TAG, "Unstable reading: Voltage = %.3fV", voltage);
        }

        // Delay between readings
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }

    // If the loop exits due to timeout
    ESP_LOGE(TAG, "DO Sensor initialization failed. Voltage did not stabilize.");
    return false;
}

ProbeSampler::ProbeSampler( const ITimeService::Ptr & timeService )
        : mDefaultCalibration( { { CFG_NUMBER_OF_SAMPLES, "1" },
                                 { CFG_TDS_CONVERSION_FACTOR_A, "421.98" },
                                 { CFG_TDS_CONVERSION_FACTOR_B, "-1080.3" },
                                 { CFG_TDS_CONVERSION_FACTOR_C, "1581.1" },
                                 { CFG_TDS_CONVERSION_FACTOR_D, "-316.71" },
                                 { CFG_PRESSURE_CONVERSION_FACTOR_A, "40.07" },
                                 { CFG_PRESSURE_CONVERSION_FACTOR_B, "-20.84" },
                                 { CFG_PH_CONVERSION_FACTOR_A, "6.465" },
                                 { CFG_PH_CONVERSION_FACTOR_B, "-9.45" },
                                 { CFG_DO_CONVERSION_FACTOR_A, "1" },
                                 { CFG_DO_CONVERSION_FACTOR_B, "0" },
                                 { CFG_FILENAME, "output.csv" }} )
        , mConfigHelper()
        , mOneWirePtr( std::make_shared<OneWire>( TEMP_SENSOR_INPUT_PIN ) )
        , mTempSensorPtr( std::make_shared<DallasTemperature>( mOneWirePtr.get() ) )
        , mTimeService( timeService )
{
    ESP_LOGI("Main", "Starting DO sensor initialization...");
    do_sensor_initialized = initialize_do_sensor(do_sensor);
    if (do_sensor_initialized) {
        ESP_LOGI("Main", "DO Sensor initialized successfully.");
    } else {
        ESP_LOGE("Main", "DO Sensor initialization failed.");
    }
    ESP_LOGI( TAG, "Instance created" );
}

float ProbeSampler::getTemperatureInCelsius () {
    mTempSensorPtr->requestTemperatures();      // Request temperature readings
    return mTempSensorPtr->getTempCByIndex(0);  // Read temperature in °C
}

float ProbeSampler::tds_temp_compensation(float tds_input_voltage, float temperature ) {
        float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0); //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
        return tds_input_voltage / compensationCoefficient; //temperature compensation
}

void clamp_result(float* result, int min_value, int max_value) {
    if (*result < min_value) {
        *result = min_value;
    } else if (*result > max_value) {
        *result = max_value;
    }
}
void ProbeSampler::readAllSensors( SampleData & data ) {
    ESP_LOGI( TAG, "Reading sensors..." );

    //Temperature
    data.temperature = getTemperatureInCelsius();

    //Pressure
    data.pressure_voltage = analog_sensor.getAnalogInputVoltage(PRESSURE_SENSOR_INPUT_PIN);
    data.pressure = analog_sensor.getValue(data.pressure_voltage, mConfigHelper->getAsFloat(CFG_PRESSURE_CONVERSION_FACTOR_A), mConfigHelper->getAsFloat(CFG_PRESSURE_CONVERSION_FACTOR_B));
    ESP_LOGI(TAG, "Pressure voltage: %f, Pressure: %f", data.pressure_voltage, data.pressure);
    delayMsec( 100 );
    //TDS
    data.tds_voltage = analog_sensor.getAnalogInputVoltage(TDS_SENSOR_INPUT_PIN);
    //data.tds = calculate_tds_from_voltage(data.tds_voltage, data.temperature, mConfigHelper->getAsFloat(CFG_TDS_CONVERSION_FACTOR_A), mConfigHelper->getAsFloat(CFG_TDS_CONVERSION_FACTOR_B));
    data.tds = analog_sensor.getValue(tds_temp_compensation(data.tds_voltage, data.temperature)
            ,mConfigHelper->getAsFloat(CFG_TDS_CONVERSION_FACTOR_A), mConfigHelper->getAsFloat(CFG_TDS_CONVERSION_FACTOR_B)
            ,mConfigHelper->getAsFloat(CFG_TDS_CONVERSION_FACTOR_C), mConfigHelper->getAsFloat(CFG_TDS_CONVERSION_FACTOR_D));
            ESP_LOGI(TAG, "TDS voltage: %f, TDS: %f", data.tds_voltage, data.tds);

    //Conductivity
    data.conductivity = data.tds/tds_to_conductivity;

    //pH
    data.ph_voltage = analog_sensor.getAnalogInputVoltage(PH_SENSOR_INPUT_PIN);
    data.ph = analog_sensor.getValue(data.ph_voltage, mConfigHelper->getAsFloat(CFG_PH_CONVERSION_FACTOR_A), mConfigHelper->getAsFloat(CFG_PH_CONVERSION_FACTOR_B));
    clamp_result(&data.ph,0,14);

    //Dissolved Oxygen
    if (do_sensor_initialized) {
    data.do2_voltage = do_sensor.getAnalogInputVoltage(DO_SENSOR_INPUT_PIN)/11; //11x gain as per sensor spec
    //data.do2 = analog_sensor.getValue(data.do2_voltage/do_calibration_voltage, mConfigHelper->getAsFloat(CFG_DO_CONVERSION_FACTOR_A), mConfigHelper->getAsFloat(CFG_DO_CONVERSION_FACTOR_B))*100;
    } else {
        data.do2_voltage = 0;
    }
    data.do2 = 100*data.do2_voltage/do_calibration_voltage;
    clamp_result(&data.do2, 0, 100);
}

SampleData::Ptr ProbeSampler::getSample() {
    int numSamples = mConfigHelper->getAsInt( CFG_NUMBER_OF_SAMPLES );
    ESP_LOGI( TAG, "Sampling data, numSamples = %d", numSamples );

    SampleData::Ptr accumulatedData = std::make_shared<SampleData>();
    accumulatedData->tsStart = mTimeService->getTime();
    accumulatedData->temperature = -127; // initialize to minimal possible value
    accumulatedData->tds = 0; //initialize to minimal possible value
    accumulatedData->conductivity = 0; //initialize to minimal possible value

    for (int i = 0; i < numSamples; ++i) {
        SampleData data;
        readAllSensors( data );
        // for temperature reading save the highest value
        if (data.temperature > accumulatedData->temperature )
        {
            accumulatedData->temperature = data.temperature;
        }
        if (data.tds > accumulatedData->tds )
        {
            accumulatedData->tds = data.tds;
            accumulatedData->conductivity = data.conductivity;
        }

        accumulatedData->pressure += data.pressure;
        accumulatedData->pressure_voltage += data.pressure_voltage;
        //accumulatedData->tds += data.tds;
        accumulatedData->tds_voltage += data.tds_voltage;
        //accumulatedData->conductivity += data.conductivity;
        accumulatedData->ph_voltage += data.ph_voltage;
        accumulatedData->ph += data.ph;
        accumulatedData->do2_voltage += data.do2_voltage;
        accumulatedData->do2 += data.do2;
        delayMsec( 200 );                               // delay 100ms between each sample
    }

    accumulatedData->pressure /= numSamples;
    accumulatedData->pressure_voltage /= numSamples;
    //accumulatedData->tds /= numSamples;
    accumulatedData->tds_voltage /= numSamples;
    //accumulatedData->conductivity /= numSamples;
    accumulatedData->ph_voltage /= numSamples;
    accumulatedData->ph /= numSamples;
    accumulatedData->do2_voltage /= numSamples;
    accumulatedData->do2 /= numSamples;

    accumulatedData->tsEnd = mTimeService->getTime();
    accumulatedData->depth = accumulatedData->pressure * pressure_to_depth; // convert pressure (psi) to depth (m)

    return accumulatedData;
}

ProbeSampler::~ProbeSampler()
{
    ESP_LOGI( TAG, "Instance destroyed" );
}

bool ProbeSampler::init( const ComponentConfig & config ) {
    ESP_LOGI( TAG, "Initializing ..." );
    mConfigHelper = std::make_shared<CComponentConfigHelper>( config, mDefaultCalibration );
    // Iterate through expected calibration parameters and print values that will be used for sampling
    ESP_LOGI( TAG, "Calibration parameters that will be applied:" );
    for ( auto & param : mDefaultCalibration )
    {
        ESP_LOGI( TAG, "%s = %s", param.first.c_str(), mConfigHelper->getAsString( param.first ).c_str() );
    }

    mTempSensorPtr->begin(); // initialize temperature sensor
    delayMsec( 1000 );

    ESP_LOGI( TAG, "Initializing complete, ready to sample" );
    return true;
}
