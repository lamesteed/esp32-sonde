#ifndef CDUMMYSAMPLER_H
#define CDUMMYSAMPLER_H

#include "ISampler.h"
#include "ITimeService.h"
#include "SampleData.h"
#include <vector>

class CDummySampler : public ISampler
{
public:
    /// @brief Constructor
    CDummySampler( const ITimeService::Ptr & systime );

    /// @brief Virtual destructor
    virtual ~CDummySampler();

private:
    // ISampler interface

    /// @brief Initialize sampler, must be called before first call to getSample()
    ///        After this call sampler considered ready to provide samples
    /// @return true if initialization successful, false otherwise
    virtual bool init( const CalibrationConfig & config ) override;

    /// @brief  Retrieve next sample from sensors
    /// @return Sample data serialized to satring,
    ///         empty string if no more samples available in current cycle (surface reached)
    virtual  SampleData::Ptr getSample() override;

    ITimeService::Ptr mTimeService; ///< Time service instance
    std::vector<SampleData::Ptr> mSamples; ///< Vector of sample data
    int mSampleIndex; ///< Current sample index

};

#endif // CDUMMYSAMPLER_H