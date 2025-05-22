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

    /// @brief Initialize sampler
    /// @param config Configuration parameters to be used for sampling
    /// @return true if initialization successful, false otherwise
    virtual bool init( const ComponentConfig & config ) override;

    /// @brief  Retrieve next sample from sensors
    /// @return Sample data, nullptr if sampling failed
    virtual  SampleData::Ptr getSample() override;

    ITimeService::Ptr mTimeService; ///< Time service instance
    std::vector<SampleData::Ptr> mSamples; ///< Vector of sample data
    int mSampleIndex; ///< Current sample index

};

#endif // CDUMMYSAMPLER_H