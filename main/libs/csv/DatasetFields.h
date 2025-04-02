#ifndef DATASETFIELDS_H
#define DATASETFIELDS_H

#include "IStorageService.h"

#include <string>
#include <vector>


struct DatasetFields {
    std::string DatasetName;
    std::string MonitoringLocationID;
    std::string MonitoringLocationName;
    std::string MonitoringLocationLatitude;
    std::string MonitoringLocationLongitude;
    std::string MonitoringLocationHorizontalCoordinateReferenceSystem;
    std::string MonitoringLocationHorizontalAccuracyMeasure;
    std::string MonitoringLocationHorizontalAccuracyUnit;
    std::string MonitoringLocationType;
    std::string ActivityType;
    std::string ActivityMediaName;
    std::string ActivityStartDate;
    std::string ActivityStartTime;
    std::string ActivityEndDate;
    std::string ActivityEndTime;
    std::string ActivityDepthHeightMeasure;
    std::string ActivityDepthHeightUnit;
    std::string SampleCollectionEquipmentName;
    std::string CharacteristicName;
    std::string MethodSpeciation;
    std::string ResultSampleFraction;
    std::string ResultValue;
    std::string ResultUnit;
    std::string ResultValueType;
    std::string ResultDetectionCondition;
    std::string ResultDetectionQuantitationLimitMeasure;
    std::string ResultDetectionQuantitationLimitUnit;
    std::string ResultDetectionQuantitationLimitType;
    std::string ResultStatusID;
    std::string ResultComment;
    std::string ResultAnalyticalMethodID;
    std::string ResultAnalyticalMethodContext;
    std::string ResultAnalyticalMethodName;
    std::string AnalysisStartDate;
    std::string AnalysisStartTime;
    std::string AnalysisEndTimeZone;
    std::string LaboratoryName;
    std::string LaboratorySampleID;

    static std::string toCSV(const DatasetFields& dataset);
    static void saveToCSV( const IStorageService::Ptr & storage, const std::vector<DatasetFields>& datasets, const std::string& filename);
};

#endif // DATASETFIELDS_H