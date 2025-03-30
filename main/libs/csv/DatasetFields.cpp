#include "DatasetFields.h"
#include <fstream>
#include <iostream>

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

void DatasetFields::saveToCSV(const std::vector<DatasetFields>& datasets, const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        // Write header
        file << "DatasetName,MonitoringLocationID,MonitoringLocationName,MonitoringLocationLatitude,MonitoringLocationLongitude,"
             << "MonitoringLocationHorizontalCoordinateReferenceSystem,MonitoringLocationHorizontalAccuracyMeasure,"
             << "MonitoringLocationHorizontalAccuracyUnit,MonitoringLocationType,ActivityType,ActivityMediaName,"
             << "ActivityStartDate,ActivityStartTime,ActivityEndDate,ActivityEndTime,ActivityDepthHeightMeasure,"
             << "ActivityDepthHeightUnit,SampleCollectionEquipmentName,CharacteristicName,MethodSpeciation,ResultSampleFraction,"
             << "ResultValue,ResultUnit,ResultValueType,ResultDetectionCondition,ResultDetectionQuantitationLimitMeasure,"
             << "ResultDetectionQuantitationLimitUnit,ResultDetectionQuantitationLimitType,ResultStatusID,ResultComment,"
             << "ResultAnalyticalMethodID,ResultAnalyticalMethodContext,ResultAnalyticalMethodName,AnalysisStartDate,"
             << "AnalysisStartTime,AnalysisEndTimeZone,LaboratoryName,LaboratorySampleID\n";

        // Write data
        for (const auto& dataset : datasets) {
            file << toCSV(dataset);
        }
        file.close();
    } else {
        std::cerr << "Unable to open file " << filename << std::endl;
    }
}