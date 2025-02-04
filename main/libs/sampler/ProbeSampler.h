
#ifndef PROBESAMPLER_H
#define PROBESAMPLER_H

#include "ISampler.h"

class ProbeSampler : public ISampler
{
public:
    /// @brief Constructor
    ProbeSampler();

    /// @brief Virtual destructor
    virtual ~ProbeSampler();

private:
    // ISampler interface

    /// @brief Initialize sampler, must be called before first call to getSample()
    ///        After this call sampler considered ready to provide samples
    /// @return true if initialization successful, false otherwise
    virtual bool init() override;

    /// @brief  Retrieve next sample from sensors
    /// @return Sample data serialized to satring,
    ///         empty string if no more samples available in current cycle (surface reached)
    virtual  std::string getSample() override;

private:
    static const char * TAG;

};

#endif // PROBESAMPLER_H