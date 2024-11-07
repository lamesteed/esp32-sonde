
#ifndef CDUMMYSAMPLER_H
#define CDUMMYSAMPLER_H

#include "ISampler.h"

class CDummySampler : public ISampler
{
public:
    /// @brief Constructor
    /// @param samples Number of samples to be generated
    CDummySampler ( const int samples );

    /// @brief Virtual destructor
    virtual ~CDummySampler();

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
    const int mSampleCounter;
    static const char * TAG;

};

#endif // CDUMMYSAMPLER_H