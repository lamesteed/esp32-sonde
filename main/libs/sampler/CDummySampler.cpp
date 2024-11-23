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
#define RHO                       997.0474 // kg/m^3 - density for fresh water (kg/m^3)
#define G                         9.80665; // gravitational constant (m/s^2)

float tempC; // temperature in Celsius
float pressure = 0;
float depth = 0;
float tds_value = 0;

OneWire oneWire(TEMP_SENSOR_INPUT_PIN);
DallasTemperature tempSensor(&oneWire);
DFRobot_ESP_EC ec;

const char * CDummySampler::TAG = "CDummySampler";

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
    if( counter > mSampleCounter )
    {
        ESP_LOGI( TAG, "getSample() - no more samples" );
        return "";
    }
    else
    {
        ESP_LOGI( TAG, "getSample() retrived sample #%d ", counter );
        tempSensor.requestTemperatures();  // Request temperature readings
        tempC = tempSensor.getTempCByIndex(0); // read temperature in °C



        int adc_value = analogRead(PRESSURE_SENSOR_INPUT_PIN);
        float pressure_voltage = ((float)adc_value * REF_VOLTAGE) / ADC_RESOLUTION;
        //pressure = 25 * pressure_voltage -12.5;
        pressure = ec.readEC(pressure_voltage, tempC); // convert voltage to EC with temperature compensation

        //pressure = (pressure_voltage - BASELINE_VOLTAGE) * (100/(4.5 - BASELINE_VOLTAGE));
        /** 
        // TODO: calculate or lookup pressure based on voltage 
        if (abs(pressure / (RHO * G) - depth) >= 1) { 
            tempSensor.requestTemperatures();       // send cmd to get temperatures
            tempC = tempSensor.getTempCByIndex(0);  // read temperature in °C
            // TODO: measure conductivity
         }
        **/
  
        // TDS measurement
        int tds_input_value = analogRead(TDS_SENSOR_INPUT_PIN);
        // read the analog value more stable by the median filtering algorithm, and convert to voltage value
        float tds_voltage = ((float)tds_input_value * REF_VOLTAGE) / ADC_RESOLUTION;
        //temperatemperatureture compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0)); 
        float compensationCoefficient = 1.0+0.02*(-25.0);
        //temperature compensation
        float compensationVoltage=tds_voltage/compensationCoefficient;
        //convert voltage value to tds value
        tds_value=(133.42*compensationVoltage*compensationVoltage*compensationVoltage - 255.86*compensationVoltage*compensationVoltage + 857.39*compensationVoltage)*0.5;

        Serial.println("=============================");
        Serial.print("Temperature: ");
        Serial.print(tempC);
        Serial.println("°C");
        Serial.print("Pressure: ");
        Serial.print(pressure);
        Serial.print("psi, ");
        Serial.print(pressure_voltage, 2);
        Serial.println("v");
        Serial.print("TDS: ");
        Serial.print(tds_value);
        Serial.print("ppm, ");
        Serial.print(tds_voltage, 2);
        Serial.println("v");

        std::string tempString = std::to_string(tempC);
        std::string pressureString = std::to_string(pressure);
        std::string pressureVoltageString = std::to_string(pressure_voltage);
        std::string tdsValueString = std::to_string(tds_value);
        std::string tdsVoltageString = std::to_string(tds_voltage);

        //std::string sampleData = "A Dummy Sample data " + std::to_string( counter++ );
        std::string sampleData = 
        "Temperature: " + tempString + 
        "°C\nPressure: " + pressureString + "psi, voltage: " + pressureVoltageString + 
        "v\nTDS: " + tdsValueString + "ppm, voltage: " + tdsVoltageString + "v\n" + 
        std::to_string( counter++ ) + "\n\n";

        return sampleData;
    }
}