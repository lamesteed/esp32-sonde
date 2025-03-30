#include "ProbeSampler.h"
#include "csv/DatasetFields.h"
#include "esp_log.h"
#include "delay.h"
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <iomanip>
#include <sstream>

#define PRESSURE_SENSOR_INPUT_PIN 36        // pin GPIO36 (ADC0) to pressure sensor
#define TDS_SENSOR_INPUT_PIN      34        // pin GPIO34 (ADC1) to TDS sensor
#define TEMP_SENSOR_INPUT_PIN     18        // pin GPIO18 to DS18B20 sensor's DATA pin
#define TOGGLE_PIN                4         // pin GPIO4 to switch between modes of operation
#define REF_VOLTAGE               5         // Maximum voltage expected at IO pins
#define BASELINE_VOLTAGE          0.5       // measured minimum voltage read from sensors
#define ADC_RESOLUTION            4096.0    // 12 bits of resolution

float ADC_COMPENSATION = 1;                 // 0dB attenuation
boolean testMode = 1;

const char * ProbeSampler::TAG = "ProbeSampler";

const char * ProbeSampler::CFG_NUMBER_OF_SAMPLES            = "NUMBER_OF_SAMPLES";
const char * ProbeSampler::CFG_TDS_CONVERSION_FACTOR_A      = "TDS_CONVERSION_FACTOR_A";
const char * ProbeSampler::CFG_TDS_CONVERSION_FACTOR_B      = "TDS_CONVERSION_FACTOR_B";
const char * ProbeSampler::CFG_PRESSURE_CONVERSION_FACTOR_A = "PRESSURE_CONVERSION_FACTOR_A";
const char * ProbeSampler::CFG_PRESSURE_CONVERSION_FACTOR_B = "PRESSURE_CONVERSION_FACTOR_B";

ProbeSampler::ProbeSampler()
        : mCalibrationParameters( { { CFG_NUMBER_OF_SAMPLES, "10" },
                                    { CFG_TDS_CONVERSION_FACTOR_A, "434.8" },
                                    { CFG_TDS_CONVERSION_FACTOR_B, "0" },
                                    { CFG_PRESSURE_CONVERSION_FACTOR_A, "25" },
                                    { CFG_PRESSURE_CONVERSION_FACTOR_B, "-12.5" } } )
        , mConfigHelper()
        , mOneWirePtr( std::make_shared<OneWire>( TEMP_SENSOR_INPUT_PIN ) )
        , mTempSensorPtr( std::make_shared<DallasTemperature>( mOneWirePtr.get() ) )
{
    ESP_LOGI( TAG, "Instance created" );
}

std::string ProbeSampler::twoDecimalString(float value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    return oss.str();
}

float ProbeSampler::getTemperatureInCelsius () {
    mTempSensorPtr->requestTemperatures();      // Request temperature readings
    return mTempSensorPtr->getTempCByIndex(0);  // Read temperature in °C
}

float ProbeSampler::getAnalogInputVoltage (int inputPin) {
    // Calculate the volts per division taking account of the chosen attenuation value.
    float input = analogRead(inputPin);
    return input * REF_VOLTAGE * ADC_COMPENSATION / ADC_RESOLUTION;
}

float ProbeSampler::getTDS (float tds_input_voltage, float temperature) {
    float k = mConfigHelper->getAsFloat( CFG_TDS_CONVERSION_FACTOR_A );
    float b = mConfigHelper->getAsFloat( CFG_TDS_CONVERSION_FACTOR_B );
    // TDS = k * V + b
    float tds = k * tds_input_voltage + b; // assuming 0v = 0ppm, 2.3v = 1000ppm.
     return (tds > 0) ? tds : 0;
}

float ProbeSampler::getConductivity (float tds_input_voltage, float temperature) {
    /* Conductivity = TDS * conversion factor, where
        0.65 - general purpose
        0.5  - sea water
        0.7  - drinking water
        0.8  - hydroponics */
    float conductivity = getTDS(tds_input_voltage, temperature)/0.7; // assuming 0.7 conversion factor
    return (conductivity > 0) ? conductivity : 0;
}

float ProbeSampler::getPressure (float pressure_input_voltage) {
        float k = mConfigHelper->getAsFloat( CFG_PRESSURE_CONVERSION_FACTOR_A );
        float b = mConfigHelper->getAsFloat( CFG_PRESSURE_CONVERSION_FACTOR_B );
        // Pressure = k * V + b
        float pressure = k * pressure_input_voltage + b; // assuming 0.5V = 0 PSI and 4.5V = 100 PSI
        return (pressure > 0) ?  pressure :  0;
}

ProbeSampler::SampleData ProbeSampler::readAllSensors() {
    Serial.println("Reading sensors...");
    SampleData data = {0, 0, 0, 0, 0, 0};;

    data.temperature = getTemperatureInCelsius();
    data.pressure_voltage = getAnalogInputVoltage(PRESSURE_SENSOR_INPUT_PIN);
    data.pressure = getPressure(data.pressure_voltage);
    data.tds_voltage = getAnalogInputVoltage(TDS_SENSOR_INPUT_PIN);
    data.tds = getTDS(data.tds_voltage, data.temperature);
    data.conductivity = getConductivity(data.tds_voltage, data.temperature);
    return data;
}

ProbeSampler::SampleData ProbeSampler::averageSensorReadings(int numSamples) {
    SampleData accumulatedData = {-127, 0, 0, 0, 0, 0};

    float temp[10];

    for (int i = 0; i < numSamples; ++i) {
        SampleData data = readAllSensors();
        temp[i] = data.temperature;
        // for temperature reading save the highest value
        if (data.temperature > accumulatedData.temperature ) {
            accumulatedData.temperature = data.temperature;
        }
        accumulatedData.pressure += data.pressure;
        accumulatedData.pressure_voltage += data.pressure_voltage;
        accumulatedData.tds += data.tds;
        accumulatedData.tds_voltage += data.tds_voltage;
        accumulatedData.conductivity += data.conductivity;
        delayMsec( 10 );                               // delay 10ms between each sample
    }

    accumulatedData.pressure /= numSamples;
    accumulatedData.pressure_voltage /= numSamples;
    accumulatedData.tds /= numSamples;
    accumulatedData.tds_voltage /= numSamples;
    accumulatedData.conductivity /= numSamples;

    // print out the temperature values to string stream
    std::ostringstream oss;
    for (int i = 0; i < numSamples; ++i) {
        oss << temp[i] << ", ";
    }
    ESP_LOGI( "averageSensorReadings", "Temperature values: %s", oss.str().c_str() );

    return accumulatedData;
}

std::string ProbeSampler::writeSampleDataInTestingMode (SampleData data, int counter) {

    std::string temperatureUnit = "deg C";
    std::string pressureUnit = "psi";
    std::string tdsUnit = "ppm";
    std::string conductivityUnit = "uS/cm"; 
    std::string datasetName = "MyDatasetName";
    std::string monitoringLocationID = "MyMonitoringLocationID";
    std::string monitoringLocationName = "MyLake";
    std::string monitoringLocationLatitude = "";
    std::string monitoringLocationLongitude = "";

    DatasetFields temperatureRow = {datasetName, monitoringLocationID, monitoringLocationName, monitoringLocationLatitude, monitoringLocationLongitude, "GPS", "0", temperatureUnit,"Lake/Pond", "Field Msr/Obs-Portable Data Logger", "Surface Water", "2018-01-30", "13:08:01", "13:08:01", "14:08:01", "0", "m", "Probe/Sensor", "Temperature, water", "", "", "-127", temperatureUnit, "Actual", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};
    DatasetFields pressureRow = {datasetName, monitoringLocationID, monitoringLocationName, monitoringLocationLatitude, monitoringLocationLongitude, "GPS", "0", pressureUnit, "Lake/Pond", "Field Msr/Obs-Portable Data Logger", "Surface Water", "2018-01-30", "13:08:01", "13:08:01", "14:08:01", "0", "m", "Probe/Sensor", "pressure", "", "", "0", pressureUnit, "Actual", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};
    DatasetFields tdsRow = {datasetName, monitoringLocationID, monitoringLocationName, monitoringLocationLatitude, monitoringLocationLongitude, "GPS", "0", tdsUnit, "Lake/Pond", "Field Msr/Obs-Portable Data Logger", "Surface Water", "2018-01-30", "13:08:01", "13:08:01", "14:08:01", "0", "m", "Probe/Sensor", "total dissolved solids", "", "", "0", tdsUnit, "Actual", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};
    DatasetFields conductivityRow = {datasetName, monitoringLocationID, monitoringLocationName, monitoringLocationLatitude, monitoringLocationLongitude, "GPS", "0", conductivityUnit, "Lake/Pond", "Field Msr/Obs-Portable Data Logger", "Surface Water", "2018-01-30", "13:08:01", "13:08:01", "14:08:01", "0", "m", "Probe/Sensor", "conductivity", "", "", "0", conductivityUnit, "Actual", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};

    // writing sample data into string to be sent out via bluetooth
    ESP_LOGI( "writeSampleDataInTestingMode", "Tfloat = %.2f, Tstr = %s", data.temperature, twoDecimalString(data.temperature).c_str()) ;
    return "Temperature: " +
    twoDecimalString(data.temperature) + "°C\nPressure: " +
    std::to_string(data.pressure) +  "psi, " +
    twoDecimalString(data.pressure_voltage) + "v\nTDS: " +
    std::to_string(data.tds) + "ppm, " +
    std::to_string(data.tds_voltage) + "v\nConductivity: " +
    std::to_string(data.conductivity) + "uS/cm\n" +
    std::to_string( counter ) + "\n\n";
    Serial.println("attributes: ");

    temperatureRow.ResultValue = twoDecimalString(data.temperature);
    pressureRow.ResultValue = std::to_string(data.pressure);
    tdsRow.ResultValue = std::to_string(data.tds);
    conductivityRow.ResultValue = std::to_string(data.conductivity);

    std::vector<DatasetFields> datasets = {temperatureRow, pressureRow, tdsRow, conductivityRow};

    DatasetFields::saveToCSV(datasets, "output.csv");
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
    pinMode(TOGGLE_PIN, INPUT);                     // initialize toggle pin as input
    testMode = digitalRead(TOGGLE_PIN);             // if 1: true, if 0: false
    ESP_LOGI( TAG, "Initializing complete, ready to sample" );
    return true;
}

std::string ProbeSampler::getSample() {
    static int counter = 1;
    if ( testMode )
    {
        ESP_LOGI(TAG, "Probe in TEST MODE");
        ESP_LOGI( TAG, "getSample retrieved sample #%d ", counter );
        return writeSampleDataInTestingMode(
                averageSensorReadings( mConfigHelper->getAsInt( CFG_NUMBER_OF_SAMPLES ) ),
                counter++ );
    } else
    {
        ESP_LOGI(TAG, "Probe in FIELD SAMPLING MODE");
        return "";
    }
}
