#ifndef ISAMPLESERIALIZER_H
#define ISAMPLESERIALIZER_H

#include "ComponentConfig.h"
#include "SampleData.h"

#include <map>

/// @brief Interface for sample serializer
class ISampleSerializer
{
public:
    using Ptr = std::shared_ptr<ISampleSerializer>;

    /// Sample collection metadata (contains details about the sample collection process: dataset name, place of collection etc)
    using MetadataConfig = std::map<std::string, std::string>;

    /// @brief Virtual destructor
    virtual ~ISampleSerializer() = default;

    /// @brief Reset serializer state and (re)initialize metadata that will be used to enrich serialized sample data
    /// @param metadata Metadata configuration to use during serialization
    virtual void reset( const ComponentConfig & metadata )  = 0;

    /// @brief Serialize next data sample to implementation-specific format
    /// @param data Sample data to be serialized
    /// @param oss Output stream to append serialized data to
    /// @return true if serialization was successful, false otherwise
    virtual bool serialize(const SampleData::Ptr & data, std::ostringstream & oss) = 0;

    /// @brief Helper function to format float value to string with two decimal places
    /// @param value float value to be formatted
    /// @return string representation of the float value with two decimal places
    ///         e.g. 1.234567 -> "1.23", -1.2 -> "-1.20", 1.0 -> "1.00"
    static std::string toTwoDecimalString( float value );
};

#endif // ISAMPLESERIALIZER_H