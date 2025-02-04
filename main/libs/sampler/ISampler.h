#ifndef ISAMPLER_H
#define ISAMPLER_H

#include <memory>
#include <string>

class ISampler
{
public:
    using Ptr = std::shared_ptr<ISampler>;

    /// @brief Virtual destructor
    virtual ~ISampler() = default;

    /// @brief Initialize sampler
    virtual bool init() = 0;

    /// @brief  Retrieve next sample from sensors
    /// @return Sample data serialized to satring,
    ///         empty string if no more samples available in current cycle (surface reached)
    virtual  std::string getSample() = 0;
};

#endif // ISAMPLER_H