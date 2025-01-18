#ifndef CTESTMODECOMMAND_H
#define CTESTMODECOMMAND_H

#include "ICommand.h"
#include "ISampler.h"
#include "IDataPublisherService.h"

/// @brief Command to perform sonde basic sensors testing
class CTestModeCommand : public ICommand
{
public:
    /// @brief Constructor
    /// @param sampler      Sampler instance (to read sensor data)
    /// @param publisher    Data publisher instance (to report execution result)
    /// @param samplesCount Number of samples to collect
    CTestModeCommand(
        const ISampler::Ptr & sampler,
        const IDataPublisherService::Ptr & publisher, int samplesCount );

    /// @brief Virtual destructor
    virtual ~CTestModeCommand() = default;

private:
    // ICommand interface
    /// @brief Get command description
    /// @return Human firendly description of the command
    virtual std::string getDescription() const override;

    /// @brief Execute the command
    /// @return execution result, true if command executed successfully, false otherwise
    virtual bool execute() override;

private:
    ISampler::Ptr mSampler;                 ///< Sampler instance
    IDataPublisherService::Ptr mPublisher;  ///< Data publisher interface
    int mSamplesCount;                      ///< Number of samples to collect
};

#endif // CTESTMODECOMMAND_H