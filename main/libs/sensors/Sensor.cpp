#include "Sensor.h"
#include "esp_log.h"
#include "delay.h"
#include <Arduino.h>
#include <iomanip>
#include <sstream>
#include <Wire.h>
#include <Adafruit_ADS1015.h>

#define REF_VOLTAGE               5         // Maximum voltage expected at IO pins
#define BASELINE_VOLTAGE          0.5       // measured minimum voltage read from sensors
#define ADC_RESOLUTION            4096.0    // 12 bits of resolution
Adafruit_ADS1115 ads(0x48);

float ADC_COMPENSATION = 1;                 // 0dB attenuation

const char * Sensor::TAG = "Sensor";

Sensor::Sensor() {
    ads.begin();
    ESP_LOGI(TAG, "Instance created");
}

float Sensor::getAnalogInputVoltage(std::string inputPin) {
    int ads_port;
    std::istringstream iss(inputPin.substr(inputPin.size() - 1));
    iss >> ads_port;
    int16_t adc = ads.readADC_SingleEnded(ads_port);
    float voltage = (adc * 0.1875)/1000;
    float result = round(voltage * 100)/100;
    return (result > 0) ? result : 0;
}

float Sensor::getAnalogInputVoltage(int inputPin) {

    float input = analogRead(inputPin);
    float voltage = input * REF_VOLTAGE * ADC_COMPENSATION / ADC_RESOLUTION;
    float result = round(voltage * 100)/100;
    return (result > 0) ? result : 0;
}

float Sensor::getValue(float input_voltage, float factorA, float factorB) {
    float result = factorA * input_voltage + factorB;
    //return (result > 0) ? result : 0;
    return result;
}

Sensor::~Sensor() {
    ESP_LOGI(TAG, "Instance destroyed");
}

bool Sensor::init() {
    return true;
}
