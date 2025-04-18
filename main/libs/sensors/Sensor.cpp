// #include "sensor.h"
// #include "esp_log.h"

// #include "CCalibrationConfigHelper.h"

// #include <Arduino.h>
// #include <OneWire.h>

// #include <iomanip>
// #include <sstream>

// const char * Sensor::TAG = "Sensor";

// Sensor::Sensor(const std::string &factorAKey, const std::string &factorBKey)
//     : mFactorAKey(factorAKey), mFactorBKey(factorBKey),mConfigHelper() {
//         ESP_LOGI( TAG, "Instance created" );
//     }

    
// float Sensor::getAnalogInputVoltage (int inputPin) {
//     // Calculate the volts per division taking account of the chosen attenuation value.
//     float input = analogRead(inputPin);
//     return input * REF_VOLTAGE * ADC_COMPENSATION / ADC_RESOLUTION;
// }

// float Sensor::calculateFromVoltage(float input_voltage, const std::string &factorAKey, const std::string &factorBKey) {
//     float k = mConfigHelper.getAsFloat(factorAKey);
//     float b = mConfigHelper.getAsFloat(factorBKey);
//     // result = k * V + b
//     float result = k * input_voltage + b;
//     return (result > 0) ? result : 0;
// }

// float Sensor::getSensorReading(float input_voltage) {
//     return Sensor::calculateFromVoltage(input_voltage, mFactorAKey, mFactorAKey);
// }

// Sensor::~Sensor()
// {
//     ESP_LOGI( TAG, "Instance destroyed" );
// }

// bool Sensor::init( const CalibrationConfig & config ) {
//     ESP_LOGI( TAG, "Initializing ..." );
//     mConfigHelper = std::make_shared<CCalibrationConfigHelper>( config, mCalibrationParameters );
//     // Iterate through expected calibration parameters and print values that will be used for sampling
//     ESP_LOGI( TAG, "Calibration parameters that will be applied:" );
//     for ( auto & param : mCalibrationParameters )
//     {
//         ESP_LOGI( TAG, "%s = %s", param.first.c_str(), mConfigHelper->getAsString( param.first ).c_str() );
//     }

//     mTempSensorPtr->begin();                             // initialize temperature sensor
//     delayMsec( 1000 );
//     ESP_LOGI( TAG, "Initializing complete, ready to sample" );
//     return true;
// }