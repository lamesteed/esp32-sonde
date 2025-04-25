#ifndef CPLAINTEXTSERIALIZER_H
#define CPLAINTEXTSERIALIZER_H

#include "ISampleSerializer.h"

class CPlainTextSerializer : public ISampleSerializer
{
public:
    /// @brief Constructor
    CPlainTextSerializer();

    /// @brief Virtual destructor
    virtual ~CPlainTextSerializer() = default;

private:

    /// @brief Reset serializer state
    virtual void reset( const MetadataConfig & metadata ) override;

    /// @brief Serialize sample data to string
    virtual bool serialize( const SampleData::Ptr & data, std::ostringstream & oss ) override;

    static const char * TAG; ///< Tag for logging
};

#endif // CPLAINTEXTSERIALIZER_H