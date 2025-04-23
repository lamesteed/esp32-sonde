#ifndef CCSVSERIALIZER_H
#define CCSVSERIALIZER_H

#include "ISampleSerializer.h"
#include "ITimeService.h"
#include "ISampler.h"
#include "CCalibrationConfigHelper.h"
#include "DatasetFields.h"

class CCsvSerializer : public ISampleSerializer
{
public:
    /// @brief Constructor
    /// @param timeService Time service interface
    CCsvSerializer(const ITimeService::Ptr & timeService );

    /// @brief Destructor
    virtual ~CCsvSerializer();

private:
    /// ISampleSerializer interface

    /// @brief Reset serializer state and (re)initialize metadata that will be used to enrich serialized sample data
    /// @param metadata Metadata configuration to use during serialization
    virtual void reset( const MetadataConfig & metadata ) override;

    /// @brief Serialize next data sample to implementation-specific format
    /// @param data Sample data to be serialized
    /// @param oss Output stream to append serialized data to
    /// @return true if serialization was successful, false otherwise
    virtual bool serialize( const SampleData::Ptr & data, std::ostringstream & oss ) override;

private:
    DatasetFields::Ptr createCsvLineTemplate() const;

    static const char * TAG; ///< Logger tag

    // Configuration parameter names
    static const char * META_DATASET_NAME;
    static const char * META_LOCATION_ID;
    static const char * META_LOCATION_NAME;
    static const char * META_LOCATION_TYPE;
    static const char * META_GPS_LATITUDE;
    static const char * META_GPS_LONGITUDE;
    static const char * META_GPS_ACCURACY;
    static const char * META_GPS_ACCURACY_UNIT;
    static const char * META_GPS_SYSTEM_NAME;


    ISampler::CalibrationParams mMetadataParameters;    ///< Expected metadata parameters and their default values
    CCalibrationConfigHelper::Ptr mConfigHelper;        ///< Calibration configuration helper
    ITimeService::Ptr mTimeService;                     ///< Time service interface
    bool mPendingHeader;                                ///< Flag indicating if header is pending
};

#endif // CCSVSERIALIZER_H