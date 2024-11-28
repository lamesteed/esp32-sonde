#include "ProbeSampler.h"
#include "esp_log.h"
#include "delay.h"
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define PRESSURE_SENSOR_INPUT_PIN 36        // pin GPIO36 (ADC0) to pressure sensor
#define TDS_SENSOR_INPUT_PIN      34        // pin GPIO34 (ADC1) to TDS sensor
#define TEMP_SENSOR_INPUT_PIN     18        // pin GPIO18 to DS18B20 sensor's DATA pin
#define REF_VOLTAGE               5         // Maximum voltage expected at IO pins
#define BASELINE_VOLTAGE          0.5       // measured minimum voltage read from sensors
#define ADC_RESOLUTION            4096.0    // 12 bits of resolution

float tempC, pressure, tds, conductivity = 0;
float ADC_COMPENSATION = 1;                 // 0dB attenuation
OneWire oneWire(TEMP_SENSOR_INPUT_PIN);
DallasTemperature tempSensor(&oneWire);

const char * ProbeSampler::TAG = "ProbeSampler";

std::string twoDecimalString(float value) {
  int whole = (int)value;                       // Extract the whole part
  int decimal = (int)((value - whole) * 100);   // Extract the decimal part
  return std::to_string(whole) + "." + (decimal < 10 ? "0" : "") + std::to_string(decimal);
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

float getTDS (float inputPin, float temperature) {
    float v = getAnalogInputVoltage(inputPin);
    float tds = 434.8 * v;                      // assuming 0v = 0ppm, 2.3v = 1000ppm.
     return (tds > 0) ? tds : 0;
}

float getConductivity (float inputPin, float temperature) {
    /* Conductivity = TDS * conversion factor, where
        0.65 - general purpose
        0.5  - sea water
        0.7  - drinking water
        0.8  - hydroponics */
    float conductivity = getTDS(inputPin, temperature)/0.7; // assuming 0.7 conversion factor
    return (conductivity > 0) ? conductivity : 0;
}

float getPressure (float inputPin) {
        float v = getAnalogInputVoltage(inputPin);
        float pressure = 25 * v -12.5; // assuming 0.5V = 0 PSI and 4.5V = 100 PSI
        return (pressure > 0) ?  pressure :  0;
}

ProbeSampler::ProbeSampler( const int samples )
    : mSampleCounter( samples ) {
    ESP_LOGI( TAG, "Instance created (samples = %d)", samples );
}

ProbeSampler::~ProbeSampler(){ESP_LOGI( TAG, "Instance destroyed" );}

bool ProbeSampler::init() {
    ESP_LOGI( TAG, "Initializing ..." );
    Serial.begin(9600);                             // initialize serial
    tempSensor.begin();                             // initialize temperature sensor
    delayMsec( 1000 );
    ESP_LOGI( TAG, "Initializing complete, ready to sample" );
    return true;
}

std::string ProbeSampler::getSample() {
    delayMsec( 1000 );
    static int counter = 1;
    std::string sampleData = "";

    if( counter > mSampleCounter ) {
        ESP_LOGI( TAG, "getSample() - no more samples" );
        return "";
    } else {
        ESP_LOGI( TAG, "getSample retrieved sample #%d ", counter );
        Serial.println("Reading sensors...");

        // reading sensors
        tempC = getTemperatureInCelsius(tempSensor);
        pressure = getPressure (PRESSURE_SENSOR_INPUT_PIN);
        tds = getTDS(TDS_SENSOR_INPUT_PIN, tempC);
        conductivity = getConductivity(TDS_SENSOR_INPUT_PIN, tempC);

        // writing sample data into string to be sent out via bluetooth
        sampleData = "Temperature: " + 
        twoDecimalString(tempC) + "°C\nPressure: " + 
        std::to_string(pressure) +  "psi, " + 
        twoDecimalString(getAnalogInputVoltage(PRESSURE_SENSOR_INPUT_PIN)) + "v\nTDS: " + 
        std::to_string(tds) + "ppm, " + 
        std::to_string(getAnalogInputVoltage(TDS_SENSOR_INPUT_PIN)) + "v\nConductivity: " + 
        std::to_string(conductivity) + "μS/cm\n" +
        std::to_string( counter++ ) + "\n\n";
        Serial.println("attributes: ");
        return sampleData;
    }
}