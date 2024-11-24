#include "CDummySampler.h"
#include "esp_log.h"
#include "delay.h"

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DFRobot_ESP_EC.h>


#define PRESSURE_SENSOR_INPUT_PIN 36 // pin GPIO36 (ADC0) to pressure sensor
#define TEMP_SENSOR_INPUT_PIN     22 // pin to DS18B20 sensor's DATA pin
#define TDS_SENSOR_INPUT_PIN      34 // pin GPI034 to TDS sensor
#define REF_VOLTAGE               5
#define BASELINE_VOLTAGE          0.52
#define ADC_RESOLUTION            4096.0
#define MAX_SENSOR_PRESSURE       100 // maximum pressure measured by pressure sensor
#define RHO                       997.0474 // kg/m^3 - density for fresh water (kg/m^3)
#define G                         9.80665; // gravitational constant (m/s^2)

float tempC = 0;
float pressure = 0;
float depth = 0;
float tds_value = 0;
DFRobot_ESP_EC ec;

OneWire oneWire(TEMP_SENSOR_INPUT_PIN);
DallasTemperature tempSensor(&oneWire);

const char * CDummySampler::TAG = "CDummySampler";


float tempMeasurement (DallasTemperature t) {
    t.requestTemperatures();  // Request temperature readings
    return tempSensor.getTempCByIndex(0); // read temperature in °C
}

float voltageMeasurement (int inputPin, int refVoltage, int adcRes) {
    int adcVal = analogRead(inputPin);
    return ((float)adcVal * refVoltage) / adcRes;
}

float ConductivMeasurement (float analogValue, float temperature) {
    //float adcCompensation = 1 + (1 / 3.9); // 1/3.9 (11dB) attenuation.
    float averageVoltage = (analogValue * REF_VOLTAGE) / ADC_RESOLUTION;
    float conductivity = ec.readEC(averageVoltage, temperature);
    return conductivity;
}

float TDSMeasurement (float analogValue, float temperature) {
        
    float adcCompensation = 1 + (1 / 3.9); // 1/3.9 (11dB) attenuation.
    float averageVoltage = (analogValue *REF_VOLTAGE/ ADC_RESOLUTION) * adcCompensation; // Calculate the volts per division using the VREF taking account of the chosen attenuation value.
    float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0); //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
    float compensationVoltage = averageVoltage / compensationCoefficient; //temperature compensation
    float tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5; //convert voltage value to tds value
    return tdsValue;
}

float pressureMeasurement (float analogValue) {
        float adcCompensation = 1 + (1 / 3.9); // 1/3.9 (11dB) attenuation.
        float pressure_voltage = adcCompensation * (analogValue * REF_VOLTAGE) / ADC_RESOLUTION;
        // assuming 0.5V = 0 PSI and 4.5V = 100 PSI
        // adjust BASELINE_VOLTAGE based on your zero-pressure reading
        //pressure = 25 * pressure_voltage -12.5;
        pressure = (pressure_voltage - BASELINE_VOLTAGE) * (MAX_SENSOR_PRESSURE/(4.5 - BASELINE_VOLTAGE));
        return pressure;
}

CDummySampler::CDummySampler( const int samples )
    : mSampleCounter( samples )
{
    ESP_LOGI( TAG, "Instance created (samples = %d)", samples );
}

CDummySampler::~CDummySampler()
{
    ESP_LOGI( TAG, "Instance destroyed" );
}

bool CDummySampler::init()
{
    ESP_LOGI( TAG, "Initializing ..." );
    
    Serial.begin(9600); // initialize serial
    tempSensor.begin(); // initialize temperature sensor
    ec.begin();         // initialize voltage coverting library

    delayMsec( 1000 );
    ESP_LOGI( TAG, "Initializing complete, ready to sample" );
    return true;
}

std::string CDummySampler::getSample()
{
    delayMsec( 1000 );
    static int counter = 1;
    std::string sampleData = "";

    if( counter > mSampleCounter )
    {
        ESP_LOGI( TAG, "getSample() - no more samples" );
        return "";
    }
    else
    {
        ESP_LOGI( TAG, "getSample() retrived sample #%d ", counter );
        //tempC = tempMeasurement(tempSensor);
        tempC = 25;
        pressure = pressureMeasurement (analogRead(PRESSURE_SENSOR_INPUT_PIN));
        float pressure_voltage = voltageMeasurement(PRESSURE_SENSOR_INPUT_PIN, REF_VOLTAGE, ADC_RESOLUTION);
        tds_value = TDSMeasurement(analogRead(TDS_SENSOR_INPUT_PIN), tempC);
        float tds_voltage = voltageMeasurement(TDS_SENSOR_INPUT_PIN, REF_VOLTAGE, ADC_RESOLUTION);
        float conductivity = ConductivMeasurement(TDS_SENSOR_INPUT_PIN, tempC);
   
       
        ESP_LOGI(TAG,"Temperature: %f°C", tempC);
        ESP_LOGI(TAG,"Pressure: %fpsi", pressure);
        ESP_LOGI(TAG,"TDS: %fppm", tds_value);
        ESP_LOGI(TAG,"Conductivity: %f", conductivity);


        // Serial.println("=============================");
        // Serial.print("Temperature: ");
        // Serial.print(tempC);
        // Serial.println("°C");
        // Serial.print("Pressure: ");
        // Serial.print(pressure);
        // Serial.print("psi, ");
        // Serial.print(pressure_voltage, 2);
        // Serial.println("v");
        // Serial.print("TDS: ");
        // Serial.print(tds_value);
        // Serial.print("ppm, ");
        // Serial.print(tds_voltage, 2);
        // Serial.println("v");

        std::string tempString = std::to_string(tempC);
        std::string pressureString = std::to_string(pressure);
        std::string pressureVoltageString = std::to_string(pressure_voltage);
        std::string tdsValueString = std::to_string(tds_value);
        std::string tdsVoltageString = std::to_string(tds_voltage);
        std::string conductivString = std::to_string(conductivity);


        sampleData = 
        "Temperature: " + tempString + 
        "°C\nPressure: " + pressureString + "psi, " + pressureVoltageString + 
        "v\nTDS: " + tdsValueString + "ppm, " + tdsVoltageString + 
        "v\nConductivity: " + conductivString +
        std::to_string( counter++ ) + "\n\n";

        return sampleData;
    }
}