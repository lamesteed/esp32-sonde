#include "CDummySampler.h"
#include "esp_log.h"
#include "delay.h"
#include <math.h>
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DFRobot_ESP_EC.h>
#include <Adafruit_ADS1015.h>
#include <string>
#include <Wire.h>

#define PRESSURE_SENSOR_INPUT_PIN 36 // pin GPIO36 (ADC0) to pressure sensor
#define TEMP_SENSOR_INPUT_PIN     17 // pin to DS18B20 sensor's DATA pin
#define ADS1115_INPUT_PIN         0  // A0 pin of ADS1115
#define REF_VOLTAGE               5
#define BASELINE_VOLTAGE          0.5
#define ADC_RESOLUTION            4096.0

float tempC = 0;
float ADC_COMPENSATION = 1; // 0dB attenuation. for 11dB: adcCompensation = 1 + (1 / 3.9)
DFRobot_ESP_EC ec;
Adafruit_ADS1115 ads;
OneWire oneWire(TEMP_SENSOR_INPUT_PIN);
DallasTemperature tempSensor(&oneWire);

const char * CDummySampler::TAG = "CDummySampler";

std::string floatToTwoDecimalString(float value) {
  int whole = (int)value; // Extract the whole part
  int decimal = (int)((value - whole) * 100); // Extract the decimal part (scaled to 2 places)
  return std::to_string(whole) + "." + (decimal < 10 ? "0" : "") + std::to_string(decimal);
}

float getTemperatureInCelsius (DallasTemperature t) {
    t.requestTemperatures();  // Request temperature readings
    return t.getTempCByIndex(0);        // read temperature in °C
}

float getAnalogInputVoltage (int inputPin) {
    // Calculate the volts per division using the VREF taking account of the chosen attenuation value.
    return (float)analogRead(inputPin) * REF_VOLTAGE * ADC_COMPENSATION / ADC_RESOLUTION;
}

float getADS1115Voltage (int inputPin) {
    
  /*GAIN_TWOTHIRDS (±6.144V): 0.1875mV per bit
    GAIN_ONE (±4.096V): 0.125mV per bit
    GAIN_TWO (±2.048V): 0.0625mV per bit
    GAIN_FOUR (±1.024V): 0.03125mV per bit
    GAIN_EIGHT (±0.512V): 0.015625mV per bit
    GAIN_SIXTEEN (±0.256V): 0.0078125mV per bit*/
    
    return (float)ads.readADC_SingleEnded(inputPin) * 0.125 / 1000; // convert adc value to voltage
}

float getTDS (float inputPin, float temperature) {
    float tds = 434.8 * getADS1115Voltage(inputPin); // assuming 0v = 0ppm, 2.3v = 1000ppm.
     return (tds > 0) ? tds : 0;
}

float getConductivity (float inputPin, float temperature) {
    /* Conductivity = TDS * conversion factor, where
        0.65 - general purpose
        0.5  - sea water
        0.7  - drinking water
        0.8  - hydroponics */
    //float conductivity = ec.readEC(getADS1115Voltage(inputPin), temperature) * 1000; // multiply by 1000 to get μS/cm
    //float conductivity = ec.readEC(ads.readADC_SingleEnded(inputPin)/10, temperature) * 1000;
    float conductivity = getTDS(inputPin, temperature)/0.7; // assuming 0.7 is the conversion factor
    return (conductivity > 0) ? conductivity : 0;
}

float getPressure (float inputPin) {
        float pressure = 25 * getAnalogInputVoltage(inputPin) -12.5; // assuming 0.5V = 0 PSI and 4.5V = 100 PSI
        return (pressure > 0) ?  pressure :  0;
}

CDummySampler::CDummySampler( const int samples )
    : mSampleCounter( samples ) {
    ESP_LOGI( TAG, "Instance created (samples = %d)", samples );
}

CDummySampler::~CDummySampler(){ESP_LOGI( TAG, "Instance destroyed" );}

bool CDummySampler::init() {
    ESP_LOGI( TAG, "Initializing ..." );
    Serial.begin(9600);         // initialize serial
    tempSensor.begin();         // initialize temperature sensor
    ec.begin();                 // initialize voltage coverting library
    ads.setGain(GAIN_ONE);      // use GAIN_ONE setting to read up to 4V
	ads.begin();                // initialize ADS1115 for conductivity measurement

    delayMsec( 1000 );
    ESP_LOGI( TAG, "Initializing complete, ready to sample" );
    return true;
}

std::string CDummySampler::getSample() {
    delayMsec( 1000 );
    static int counter = 1;
    std::string sampleData = "";

    if( counter > mSampleCounter ) {
        ESP_LOGI( TAG, "getSample() - no more samples" );
        return "";
    } else {
        ESP_LOGI( TAG, "getSample() retrived sample #%d ", counter );
        tempC = getTemperatureInCelsius(tempSensor);
        sampleData = "Temperature: " + 
        floatToTwoDecimalString(tempC) + "°C\nPressure: " + 
        std::to_string(getPressure (PRESSURE_SENSOR_INPUT_PIN)) +  "psi, " + 
        floatToTwoDecimalString(getAnalogInputVoltage(PRESSURE_SENSOR_INPUT_PIN)) + "v\nTDS: " + 
        std::to_string(getTDS(ADS1115_INPUT_PIN, tempC)) + "ppm, " + 
        floatToTwoDecimalString(getADS1115Voltage(ADS1115_INPUT_PIN)) + "v\nConductivity: " + 
        std::to_string(getConductivity(ADS1115_INPUT_PIN, tempC)) + "μS/cm\n" +
        std::to_string( counter++ ) + "\n\n";
        return sampleData;
    }
}