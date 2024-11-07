#ifndef ISAMPLER_H
#define ISAMPLER_H

#include <string>

class ISampler
{
public:
    /// @brief Virtual destructor
    virtual ~ISampler()
    {
    }

    /// @brief Initialize sampler
    virtual bool init() = 0;

    /// @brief  Retrieve next sample from sensors
    /// @return Sample data serialized to satring,
    ///         empty string if no more samples available in current cycle (surface reached)
    virtual  std::string getSample() = 0;
};

#endif // ISAMPLER_H