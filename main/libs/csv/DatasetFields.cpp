#include "DatasetFields.h"
#include <sstream>

void DatasetFields::toCSV( std::ostringstream & oss, const DatasetFields::Ptr & dataset )
{
    oss << dataset->DatasetName << ","
        << dataset->MonitoringLocationID << ","
        << dataset->MonitoringLocationName << ","
        << dataset->MonitoringLocationLatitude << ","
        << dataset->MonitoringLocationLongitude << ","
        << dataset->MonitoringLocationHorizontalCoordinateReferenceSystem << ","
        << dataset->MonitoringLocationHorizontalAccuracyMeasure << ","
        << dataset->MonitoringLocationHorizontalAccuracyUnit << ","
        << dataset->MonitoringLocationType << ","
        << dataset->ActivityType << ","
        << dataset->ActivityMediaName << ","
        << dataset->ActivityStartDate << ","
        << dataset->ActivityStartTime << ","
        << dataset->ActivityEndDate << ","
        << dataset->ActivityEndTime << ","
        << dataset->ActivityDepthHeightMeasure << ","
        << dataset->ActivityDepthHeightUnit << ","
        << dataset->SampleCollectionEquipmentName << ","
        << dataset->CharacteristicName << ","
        << dataset->MethodSpeciation << ","
        << dataset->ResultSampleFraction << ","
        << dataset->ResultValue << ","
        << dataset->ResultUnit << ","
        << dataset->ResultValueType << ","
        << dataset->ResultDetectionCondition << ","
        << dataset->ResultDetectionQuantitationLimitMeasure << ","
        << dataset->ResultDetectionQuantitationLimitUnit << ","
        << dataset->ResultDetectionQuantitationLimitType << ","
        << dataset->ResultStatusID << ","
        << dataset->ResultComment << ","
        << dataset->ResultAnalyticalMethodID << ","
        << dataset->ResultAnalyticalMethodContext << ","
        << dataset->ResultAnalyticalMethodName << ","
        << dataset->AnalysisStartDate << ","
        << dataset->AnalysisStartTime << ","
        << dataset->AnalysisEndTimeZone << ","
        << dataset->LaboratoryName << ","
        << dataset->LaboratorySampleID
        << "\n";
}

void DatasetFields::writeCsvHeader( std::ostringstream & oss )
{
    // Write header
    oss << "DatasetName,MonitoringLocationID,MonitoringLocationName,"
        << "MonitoringLocationLatitude,MonitoringLocationLongitude,"
        << "MonitoringLocationHorizontalCoordinateReferenceSystem,"
        << "MonitoringLocationHorizontalAccuracyMeasure,"
        << "MonitoringLocationHorizontalAccuracyUnit,MonitoringLocationType,ActivityType,"
        << "ActivityMediaName,ActivityStartDate,ActivityStartTime,ActivityEndDate,"
        << "ActivityEndTime,ActivityDepthHeightMeasure,"
        << "ActivityDepthHeightUnit,SampleCollectionEquipmentName,CharacteristicName,"
        << "MethodSpeciation,ResultSampleFraction,"
        << "ResultValue,ResultUnit,ResultValueType,ResultDetectionCondition,"
        << "ResultDetectionQuantitationLimitMeasure,"
        << "ResultDetectionQuantitationLimitUnit,ResultDetectionQuantitationLimitType,"
        << "ResultStatusID,ResultComment,"
        << "ResultAnalyticalMethodID,ResultAnalyticalMethodContext,ResultAnalyticalMethodName,"
        << "AnalysisStartDate,"
        << "AnalysisStartTime,AnalysisEndTimeZone,LaboratoryName,LaboratorySampleID"
        << "\n";
}