#ifndef CDUMMYSAMPLER_H
#define CDUMMYSAMPLER_H

#include "ISampler.h"

class CDummySampler : public ISampler
{
public:
    /// @brief Constructor
    CDummySampler();

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
    virtual  std::string getSample() override;
};

#endif // CDUMMYSAMPLER_H