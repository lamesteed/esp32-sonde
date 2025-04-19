#include "Sensor.h"
#include "DatasetFields.h"
#include "CSystemTimeService.h"
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
#define PH_SENSOR_INPUT_PIN       26        // pin GPIO26 to PH sensor
#define DO_SENSOR_INPUT_PIN       27        // pin GPIO27 to DO sensor
#define REF_VOLTAGE               5         // Maximum voltage expected at IO pins
#define BASELINE_VOLTAGE          0.5       // measured minimum voltage read from sensors
#define ADC_RESOLUTION            4096.0    // 12 bits of resolution

float ADC_COMPENSATION = 1;                 // 0dB attenuation

const char * Sensor::TAG = "Sensor";

Sensor::Sensor()
{
    ESP_LOGI(TAG, "Instance created");
}

float Sensor::getAnalogInputVoltage(int inputPin) {
    // Calculate the volts per division taking account of the chosen attenuation value.
    float input = analogRead(inputPin);
    return input * REF_VOLTAGE * ADC_COMPENSATION / ADC_RESOLUTION;
}

float Sensor::getValue(float input_voltage, float factorA, float factorB) {
    // Perform the calculation using the provided factors
    float result = factorA * input_voltage + factorB;
    return (result > 0) ? result : 0;
}

Sensor::~Sensor() {
    ESP_LOGI(TAG, "Instance destroyed");
}

bool Sensor::init( const CalibrationConfig & config ) {

    return true;
}
