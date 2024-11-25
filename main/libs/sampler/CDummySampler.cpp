#include "CDummySampler.h"
#include "esp_log.h"
#include "delay.h"

#include <math.h>
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DFRobot_ESP_EC.h>
#include <Adafruit_ADS1015.h>


#define PRESSURE_SENSOR_INPUT_PIN 36 // pin GPIO36 (ADC0) to pressure sensor
#define TEMP_SENSOR_INPUT_PIN     22 // pin to DS18B20 sensor's DATA pin
#define TDS_SENSOR_INPUT_PIN      34 // pin GPI034 to TDS sensor
#define REF_VOLTAGE               5
#define BASELINE_VOLTAGE          0.5
#define ADC_RESOLUTION            4096.0
#define MAX_SENSOR_PRESSURE       100 // maximum pressure measured by pressure sensor
#define RHO                       997.0474 // kg/m^3 - density for fresh water (kg/m^3)
#define G                         9.80665; // gravitational constant (m/s^2)

float tempC = 0;
float pressure = 0;
float depth = 0;
float tds_value = 0;
float conductivity = 0;
float adcCompensation = 1; // 0dB attenuation. for 11dB: adcCompensation = 1 + (1 / 3.9)

DFRobot_ESP_EC ec;
//Adafruit_ADS1115 ads;

OneWire oneWire(TEMP_SENSOR_INPUT_PIN);
DallasTemperature tempSensor(&oneWire);

const char * CDummySampler::TAG = "CDummySampler";


float tempMeasurement (DallasTemperature t) {
    t.requestTemperatures();  // Request temperature readings
    return roundf(t.getTempCByIndex(0)*100/100); // read temperature in °C
}

float voltageMeasurement (int inputPin, int refVoltage, int adcRes) {
    int adcVal = analogRead(inputPin);
    // Calculate the volts per division using the VREF taking account of the chosen attenuation value.
    return roundf(100*((float)adcVal * refVoltage) * adcCompensation / adcRes)/100;
}

float ConductivMeasurement (float inputPin, float temperature) {
    float voltage = voltageMeasurement(inputPin, REF_VOLTAGE, ADC_RESOLUTION);
    //float voltage = voltageMeasurement(ads.readADC_SingleEnded(2)/10,refVoltage,adcRes);
    return roundf(ec.readEC(voltage, temperature)*100)/100;
}

float TDSMeasurement (float inputPin, float temperature) {
        
    float voltage = voltageMeasurement(inputPin, REF_VOLTAGE, ADC_RESOLUTION);
    return roundf(434.8 * voltage *100)/100; // assuming 0v = 0ppm, 2.3v = 1000ppm.
}

float pressureMeasurement (float inputPin) {
        float voltage = voltageMeasurement(inputPin, REF_VOLTAGE, ADC_RESOLUTION);
        pressure = roundf((25 * voltage -12.5)*100)/100; // assuming 0.5V = 0 PSI and 4.5V = 100 PSI
        return (pressure < 0) ?  0 :  pressure;
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
    //ads.setGain(GAIN_ONE); 
	//ads.begin();        // initialize ADS1115 for conductivity measurement

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
        tempC = tempMeasurement(tempSensor);
        pressure = pressureMeasurement (PRESSURE_SENSOR_INPUT_PIN);
        float pressure_voltage = voltageMeasurement(PRESSURE_SENSOR_INPUT_PIN, REF_VOLTAGE, ADC_RESOLUTION);
        tds_value = TDSMeasurement(TDS_SENSOR_INPUT_PIN, tempC);
        float tds_voltage = voltageMeasurement(TDS_SENSOR_INPUT_PIN, REF_VOLTAGE, ADC_RESOLUTION);
        conductivity = ConductivMeasurement(TDS_SENSOR_INPUT_PIN, tempC);
   
       
        ESP_LOGI(TAG,"Temperature: %f°C", tempC);
        ESP_LOGI(TAG,"Pressure: %fpsi", pressure);
        ESP_LOGI(TAG,"TDS: %fppm", tds_value);
        ESP_LOGI(TAG,"Conductivity: %f", conductivity);

        sampleData = 
        "Temperature: " + std::to_string(tempC) + 
        "°C\nPressure: " + std::to_string(pressure) 
        + "psi, " + std::to_string(pressure_voltage) + 
        "v\nTDS: " + std::to_string(tds_value) + 
        "ppm, " + std::to_string(tds_voltage) + 
        "v\nConductivity: " + std::to_string(conductivity) + 
        "mS/cm\n" +
        std::to_string( counter++ ) + "\n\n";

        return sampleData;
    }
}