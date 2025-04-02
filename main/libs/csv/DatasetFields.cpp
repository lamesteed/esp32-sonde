#include "DatasetFields.h"
#include <sstream>
#include "esp_log.h"

std::string DatasetFields::toCSV(const DatasetFields& dataset) {
    return dataset.DatasetName + "," + dataset.MonitoringLocationID + "," + dataset.MonitoringLocationName + "," +
           dataset.MonitoringLocationLatitude + "," + dataset.MonitoringLocationLongitude + "," +
           dataset.MonitoringLocationHorizontalCoordinateReferenceSystem + "," +
           dataset.MonitoringLocationHorizontalAccuracyMeasure + "," + dataset.MonitoringLocationHorizontalAccuracyUnit + "," +
           dataset.MonitoringLocationType + "," + dataset.ActivityType + "," + dataset.ActivityMediaName + "," +
           dataset.ActivityStartDate + "," + dataset.ActivityStartTime + "," + dataset.ActivityEndDate + "," +
           dataset.ActivityEndTime + "," + dataset.ActivityDepthHeightMeasure + "," + dataset.ActivityDepthHeightUnit + "," +
           dataset.SampleCollectionEquipmentName + "," + dataset.CharacteristicName + "," + dataset.MethodSpeciation + "," +
           dataset.ResultSampleFraction + "," + dataset.ResultValue + "," + dataset.ResultUnit + "," + dataset.ResultValueType + "," +
           dataset.ResultDetectionCondition + "," + dataset.ResultDetectionQuantitationLimitMeasure + "," +
           dataset.ResultDetectionQuantitationLimitUnit + "," + dataset.ResultDetectionQuantitationLimitType + "," +
           dataset.ResultStatusID + "," + dataset.ResultComment + "," + dataset.ResultAnalyticalMethodID + "," +
           dataset.ResultAnalyticalMethodContext + "," + dataset.ResultAnalyticalMethodName + "," + dataset.AnalysisStartDate + "," +
           dataset.AnalysisStartTime + "," + dataset.AnalysisEndTimeZone + "," + dataset.LaboratoryName + "," +
           dataset.LaboratorySampleID + "\n";
}

void DatasetFields::saveToCSV(const IStorageService::Ptr & storage, const std::vector<DatasetFields>& datasets, const std::string& filename) {
    std::ostringstream oss;
        // Write header
        oss << "DatasetName,MonitoringLocationID,MonitoringLocationName,";
        oss << "MonitoringLocationLatitude,MonitoringLocationLongitude,";
        oss << "MonitoringLocationHorizontalCoordinateReferenceSystem,";
        oss << "MonitoringLocationHorizontalAccuracyMeasure,";
        oss << "MonitoringLocationHorizontalAccuracyUnit,MonitoringLocationType,ActivityType,";
        oss << "ActivityMediaName,ActivityStartDate,ActivityStartTime,ActivityEndDate,";
        oss << "ActivityEndTime,ActivityDepthHeightMeasure,";
        oss << "ActivityDepthHeightUnit,SampleCollectionEquipmentName,CharacteristicName,";
        oss << "MethodSpeciation,ResultSampleFraction,";
        oss << "ResultValue,ResultUnit,ResultValueType,ResultDetectionCondition,";
        oss << "ResultDetectionQuantitationLimitMeasure,";
        oss << "ResultDetectionQuantitationLimitUnit,ResultDetectionQuantitationLimitType,";
        oss << "ResultStatusID,ResultComment,";
        oss << "ResultAnalyticalMethodID,ResultAnalyticalMethodContext,ResultAnalyticalMethodName,";
        oss << "AnalysisStartDate,";
        oss << "AnalysisStartTime,AnalysisEndTimeZone,LaboratoryName,LaboratorySampleID\n";
        oss.flush();
        // Write data
        //for (const auto& dataset : datasets) {
        //    file << toCSV(dataset);
        //}
        //file <<  DatasetFields::toCSV(datasets.front());
        ESP_LOGI( "DF", "before");
        ESP_LOGI( "DF", "%s", oss.str().c_str() );
        ESP_LOGI( "DF", "done" );
        //storage->storeData(filename, file.str());
}