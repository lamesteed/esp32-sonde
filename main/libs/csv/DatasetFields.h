#ifndef DATASETFIELDS_H
#define DATASETFIELDS_H

#include <memory>
#include <string>
#include <vector>
#include <sstream>

struct DatasetFields {
    using Ptr = std::shared_ptr<DatasetFields>;
    using ListPtr = std::vector<DatasetFields::Ptr>;

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

    static void writeCsvHeader( std::ostringstream & oss );
    static void toCSV( std::ostringstream & oss, const DatasetFields::Ptr & dataset );
};

#endif // DATASETFIELDS_H