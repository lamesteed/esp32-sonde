#ifndef ISAMPLER_H
#define ISAMPLER_H

#include "ComponentConfig.h"
#include "SampleData.h"

#include <memory>
#include <string>
#include <map>

class ISampler
{
public:
    using Ptr = std::shared_ptr<ISampler>;

    /// @brief Virtual destructor
    virtual ~ISampler() = default;

    /// @brief Initialize sampler
    /// @param config Configuration parameters to be used for sampling
    /// @return true if initialization successful, false otherwise
    virtual bool init( const ComponentConfig & config ) = 0;

    /// @brief  Retrieve next sample from sensors
    /// @return Sample data, nullptr if sampling failed
    virtual  SampleData::Ptr getSample() = 0;
};

#endif // ISAMPLER_H