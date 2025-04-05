#include "DatasetFields.h"
#include <sstream>
#include "esp_log.h"

std::string DatasetFields::toCSV(const DatasetFields& dataset) {
    return dataset.DatasetName + "," + dataset.MonitoringLocationID + "," + dataset.MonitoringLocationName + "," + "\n";
}

void DatasetFields::saveToCSV(const IStorageService::Ptr & storage, const std::vector<DatasetFields>& datasets, const std::string& filename) {
    std::ostringstream oss;
        // Write header
        oss << "DatasetName,MonitoringLocationID,MonitoringLocationName,";
        // oss << "MonitoringLocationLatitude,MonitoringLocationLongitude,";
        // oss << "MonitoringLocationHorizontalCoordinateReferenceSystem,";
        // oss << "MonitoringLocationHorizontalAccuracyMeasure,";
        // oss << "MonitoringLocationHorizontalAccuracyUnit,MonitoringLocationType,ActivityType,";
        // oss << "ActivityMediaName,ActivityStartDate,ActivityStartTime,ActivityEndDate,";
        // oss << "ActivityEndTime,ActivityDepthHeightMeasure,";
        // oss << "ActivityDepthHeightUnit,SampleCollectionEquipmentName,CharacteristicName,";
        // oss << "MethodSpeciation,ResultSampleFraction,";
        // oss << "ResultValue,ResultUnit,ResultValueType,ResultDetectionCondition,";
        // oss << "ResultDetectionQuantitationLimitMeasure,";
        // oss << "ResultDetectionQuantitationLimitUnit,ResultDetectionQuantitationLimitType,";
        // oss << "ResultStatusID,ResultComment,";
        // oss << "ResultAnalyticalMethodID,ResultAnalyticalMethodContext,ResultAnalyticalMethodName,";
        // oss << "AnalysisStartDate,";
        // oss << "AnalysisStartTime,AnalysisEndTimeZone,LaboratoryName,LaboratorySampleID";
        oss << "\n";
        oss.flush();
        // Write data
        for (const auto& dataset : datasets) {
           oss << toCSV(dataset);
        }
        //oss <<  DatasetFields::toCSV(datasets.front());
        ESP_LOGI( "DF", "before");
        ESP_LOGI( "DF", "%s", oss.str().c_str() );
        ESP_LOGI( "DF", "done" );
        storage->storeData(filename, oss.str());
}