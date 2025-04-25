#include "CCsvSerializer.h"
#include "esp_log.h"

#include <sstream>

// constants definition
const char * CCsvSerializer::TAG = "CCsvSerializer";

const char * CCsvSerializer::META_DATASET_NAME =        "DATASET_NAME";
const char * CCsvSerializer::META_LOCATION_ID =         "LOCATION_ID";
const char * CCsvSerializer::META_LOCATION_NAME =       "LOCATION_NAME";
const char * CCsvSerializer::META_LOCATION_TYPE =       "LOCATION_TYPE";
const char * CCsvSerializer::META_GPS_LATITUDE =        "GPS_LATITUDE";
const char * CCsvSerializer::META_GPS_LONGITUDE =       "GPS_LONGITUDE";
const char * CCsvSerializer::META_GPS_ACCURACY =        "GPS_ACCURACY";
const char * CCsvSerializer::META_GPS_ACCURACY_UNIT =   "GPS_ACCURACY_UNIT";
const char * CCsvSerializer::META_GPS_SYSTEM_NAME =     "GPS_SYSTEM_NAME";

CCsvSerializer::CCsvSerializer(
        const ITimeService::Ptr & timeService)
    : mMetadataParameters( { { META_DATASET_NAME, "UNKNOWN" },
                             { META_LOCATION_ID, "UNKNOWN_LOCATION_ID" },
                             { META_LOCATION_NAME, "UNKNOWN_LOCATION_NAME" },
                             { META_LOCATION_TYPE, "Lake/Pond" },
                             { META_GPS_LATITUDE, "0.0" },
                             { META_GPS_LONGITUDE, "0.0" },
                             { META_GPS_ACCURACY, "10.0" },
                             { META_GPS_ACCURACY_UNIT, "m" },
                             { META_GPS_SYSTEM_NAME, "WGS84" } } )
    , mConfigHelper( nullptr )
    , mTimeService( timeService)
    , mPendingHeader( true )
{
    ESP_LOGI( TAG, "Instance created" );
}

CCsvSerializer::~CCsvSerializer()
{
    ESP_LOGI( TAG, "Instance destroyed" );
}

void CCsvSerializer::reset( const MetadataConfig & metadata )
{
    ESP_LOGI( TAG, "reset() - resetting serializer state" );

    // Recreate metadata configuration helper with new metadata
    mConfigHelper = std::make_shared<CCalibrationConfigHelper>( metadata, mMetadataParameters );

    // Reset the pending header flag
    mPendingHeader = true;
}

DatasetFields::Ptr CCsvSerializer::createCsvLineTemplate() const
{
    DatasetFields::Ptr data = std::make_shared<DatasetFields>();
    data->DatasetName = mConfigHelper->getAsString( META_DATASET_NAME );
    data->MonitoringLocationID = mConfigHelper->getAsString( META_LOCATION_ID );
    data->MonitoringLocationName = mConfigHelper->getAsString( META_LOCATION_NAME );
    data->MonitoringLocationLatitude = mConfigHelper->getAsString( META_GPS_LATITUDE );
    data->MonitoringLocationLongitude = mConfigHelper->getAsString( META_GPS_LONGITUDE );
    data->MonitoringLocationHorizontalCoordinateReferenceSystem = mConfigHelper->getAsString( META_GPS_SYSTEM_NAME );
    data->MonitoringLocationHorizontalAccuracyMeasure = mConfigHelper->getAsString( META_GPS_ACCURACY );
    data->MonitoringLocationHorizontalAccuracyUnit = mConfigHelper->getAsString( META_GPS_ACCURACY_UNIT );
    data->MonitoringLocationType = mConfigHelper->getAsString( META_LOCATION_TYPE );
    data->ActivityType = "Field Msr/Obs-Portable Data Logger";
    data->ActivityMediaName = "Surface Water";
    // skipping ActivityStartDate;
    // skipping ActivityStartTime;
    // skipping ActivityEndDate;
    // skipping ActivityEndTime;
    // skipping ActivityDepthHeightMeasure;
    data->ActivityDepthHeightUnit = "m";
    data->SampleCollectionEquipmentName = "Probe/Sensor";
    // skipping CharacteristicName (e.g. "Temperature, water"
    // skipping MethodSpeciation
    // skipping ResultSampleFraction
    // skipping ResultValue
    // skipping ResultUnit
    data->ResultValueType = "Actual";
    // skipping ResultValueType;
    // skipping ResultDetectionCondition;
    // skipping ResultDetectionQuantitationLimitMeasure;
    // skipping ResultDetectionQuantitationLimitUnit;
    // skipping ResultDetectionQuantitationLimitType;
    // skipping ResultStatusID;
    // skipping ResultComment;
    // skipping ResultAnalyticalMethodID;
    // skipping ResultAnalyticalMethodContext;
    // skipping ResultAnalyticalMethodName;
    // skipping AnalysisStartDate;
    // skipping AnalysisStartTime;
    // skipping AnalysisEndTimeZone;
    // skipping LaboratoryName;
    // skipping LaboratorySampleID;

    return data;
}

bool CCsvSerializer::serialize( const SampleData::Ptr & data, std::ostringstream & oss )
{
    ESP_LOGI( TAG, "serialize() - serializing next sample" );

    if( !mConfigHelper )
    {
        ESP_LOGE( TAG, "serialize() - metadata configuration is not set" );
        return false;
    }

    if ( mPendingHeader )
    {
        ESP_LOGI( TAG, "serialize() - serializing header" );
        DatasetFields::writeCsvHeader( oss );
        mPendingHeader = false;

    }

    // Prepare CSV line template with common fields set
    DatasetFields::Ptr csvFields = createCsvLineTemplate();
    csvFields->ActivityStartDate = mTimeService->toString(data->tsStart, "%Y-%m-%d");
    csvFields->ActivityStartTime = mTimeService->toString(data->tsStart, "%H:%M:%S");
    csvFields->ActivityEndDate = mTimeService->toString(data->tsEnd, "%Y-%m-%d");
    csvFields->ActivityEndTime = mTimeService->toString(data->tsEnd, "%H:%M:%S");
    csvFields->ActivityDepthHeightMeasure = ISampleSerializer::toTwoDecimalString( data->depth );

    // Set temperature-related fields and serialize
    csvFields->CharacteristicName = "\"Temperature, water\"";
    csvFields->ResultValue = ISampleSerializer::toTwoDecimalString( data->temperature );
    csvFields->ResultUnit = "deg C";
    DatasetFields::toCSV( oss, csvFields );

    // Set pressure-related fields and serialize
    csvFields->CharacteristicName = "Pressure";
    csvFields->ResultValue = std::to_string( data->pressure );
    csvFields->ResultUnit = "psi";
    DatasetFields::toCSV( oss, csvFields );

    // Set TDS-related fields and serialize
    csvFields->CharacteristicName = "Total dissolved solids";
    csvFields->ResultValue = std::to_string( data->tds );
    csvFields->ResultUnit = "ppm";
    DatasetFields::toCSV( oss, csvFields );

    // Set conductivity-related fields and serialize
    csvFields->CharacteristicName = "Conductivity";
    csvFields->ResultValue = std::to_string( data->conductivity );
    csvFields->ResultUnit = "uS/cm";
    DatasetFields::toCSV( oss, csvFields );

    // Set pH-related fields and serialize
    csvFields->CharacteristicName = "pH";
    csvFields->ResultValue = std::to_string( data->ph );
    csvFields->ResultUnit = "None";
    DatasetFields::toCSV( oss, csvFields );

    // Set DO-related fields and serialize
    csvFields->CharacteristicName = "Dissolved oxygen (DO)";
    csvFields->ResultValue = std::to_string( data->do2 );
    csvFields->ResultUnit = "mg/l";
    DatasetFields::toCSV( oss, csvFields );

    return true;
}

