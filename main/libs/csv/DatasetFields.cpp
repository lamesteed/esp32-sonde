#include "DatasetFields.h"
#include <sstream>

void DatasetFields::toCSV( std::ostringstream & oss, const DatasetFields::Ptr & dataset )
{
    oss << dataset->DatasetName << ","
        << dataset->MonitoringLocationID << ","
        << dataset->MonitoringLocationName << ",";
}

void DatasetFields::saveToCSV(const IStorageService::Ptr & storage, const DatasetFields::ListPtr & datasets, const std::string & filename)
{
    std::ostringstream oss;
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

    // Write data fields
    for (const auto & dataset : datasets ) {
       toCSV( oss, dataset );
    }

    // Store the CSV data in the storage service
    storage->storeData( filename, oss.str() );
}