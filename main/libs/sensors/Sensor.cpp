#include "Sensor.h"
#include "esp_log.h"
#include "delay.h"
#include <Arduino.h>
#include <iomanip>
#include <sstream>

#define REF_VOLTAGE               5         // Maximum voltage expected at IO pins
#define BASELINE_VOLTAGE          0.5       // measured minimum voltage read from sensors
#define ADC_RESOLUTION            4096.0    // 12 bits of resolution

float ADC_COMPENSATION = 1;                 // 0dB attenuation

const char * Sensor::TAG = "Sensor";

Sensor::Sensor() {
    ESP_LOGI(TAG, "Instance created");
}

float Sensor::getAnalogInputVoltage(int inputPin) {
    float input = analogRead(inputPin);
    return input * REF_VOLTAGE * ADC_COMPENSATION / ADC_RESOLUTION;
}

float Sensor::getValue(float input_voltage, float factorA, float factorB) {
    float result = factorA * input_voltage + factorB;
    return (result > 0) ? result : 0;
}

Sensor::~Sensor() {
    ESP_LOGI(TAG, "Instance destroyed");
}

bool Sensor::init() {
    return true;
}
