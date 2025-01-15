#include "ProbeSampler.h"
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

struct SampleData {
    float temperature;
    float pressure_voltage;
    float pressure;
    float tds_voltage;
    float tds;
    float conductivity;
};

float ADC_COMPENSATION = 1;                 // 0dB attenuation
boolean testMode = 1;
OneWire oneWire(TEMP_SENSOR_INPUT_PIN);
DallasTemperature tempSensor(&oneWire);

const char * ProbeSampler::TAG = "ProbeSampler";

std::string twoDecimalString(float value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    return oss.str();
}

float getTemperatureInCelsius (DallasTemperature t) {
    t.requestTemperatures();                    // Request temperature readings
    return t.getTempCByIndex(0);                // read temperature in °C
}

float getAnalogInputVoltage (int inputPin) {
    // Calculate the volts per division taking account of the chosen attenuation value.
    float input = analogRead(inputPin);
    return input * REF_VOLTAGE * ADC_COMPENSATION / ADC_RESOLUTION;
}

float getTDS (float tds_input_voltage, float temperature) {
    float tds = 434.8 * tds_input_voltage;                      // assuming 0v = 0ppm, 2.3v = 1000ppm.
     return (tds > 0) ? tds : 0;
}

float getConductivity (float tds_input_voltage, float temperature) {
    /* Conductivity = TDS * conversion factor, where
        0.65 - general purpose
        0.5  - sea water
        0.7  - drinking water
        0.8  - hydroponics */
    float conductivity = getTDS(tds_input_voltage, temperature)/0.7; // assuming 0.7 conversion factor
    return (conductivity > 0) ? conductivity : 0;
}

float getPressure (float pressure_input_voltage) {
        float pressure = 25 * pressure_input_voltage -12.5; // assuming 0.5V = 0 PSI and 4.5V = 100 PSI
        return (pressure > 0) ?  pressure :  0;
}

SampleData readAllSensors() {
    Serial.println("Reading sensors...");
    SampleData data = {0, 0, 0, 0, 0, 0};;

    data.temperature = getTemperatureInCelsius(tempSensor);
    data.pressure_voltage = getAnalogInputVoltage(PRESSURE_SENSOR_INPUT_PIN);
    data.pressure = getPressure(data.pressure_voltage);
    data.tds_voltage = getAnalogInputVoltage(TDS_SENSOR_INPUT_PIN);
    data.tds = getTDS(data.tds_voltage, data.temperature);
    data.conductivity = getConductivity(data.tds_voltage, data.temperature);
    return data;
}

SampleData averageSensorReadings(int numSamples) {
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
        delayMsec( 100 );                               // delay 10ms between each sample
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

std::string writeSampleDataInTestingMode (SampleData data, int counter) {

    // writing sample data into string to be sent out via bluetooth
    ESP_LOGI( "writeSampleDataInTestingMode", "Tfloat = %.2f, Tstr = %s", data.temperature, twoDecimalString(data.temperature).c_str()) ;
    return "Temperature: " +
    twoDecimalString(data.temperature) + "°C\nPressure: " +
    std::to_string(data.pressure) +  "psi, " +
    twoDecimalString(data.pressure_voltage) + "v\nTDS: " +
    std::to_string(data.tds) + "ppm, " +
    std::to_string(data.tds_voltage) + "v\nConductivity: " +
    std::to_string(data.conductivity) + "μS/cm\n" +
    std::to_string( counter ) + "\n\n";
    Serial.println("attributes: ");
}

ProbeSampler::ProbeSampler( const int samples )
    : mSampleCounter( samples ) {
    ESP_LOGI( TAG, "Instance created (samples = %d)", samples );
}

ProbeSampler::~ProbeSampler(){ESP_LOGI( TAG, "Instance destroyed" );}

bool ProbeSampler::init() {
    ESP_LOGI( TAG, "Initializing ..." );
    tempSensor.begin();                             // initialize temperature sensor
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
        return writeSampleDataInTestingMode( averageSensorReadings( 10 ), counter++ );
    } else
    {
        ESP_LOGI(TAG, "Probe in FIELD SAMPLING MODE");
        return "";
    }
}
