#ifndef CTESTMODECOMMAND_H
#define CTESTMODECOMMAND_H

#include "ICommand.h"
#include "ISampler.h"
#include "ISampleSerializer.h"
#include "IDataPublisherService.h"
#include "IStorageService.h"
#include "ComponentConfig.h"

/// @brief Command to perform sonde basic sensors testing
class CTestModeCommand : public ICommand
{
public:
    /// @brief Constructor
    /// @param sampler      Sampler instance (to read sensor data)
    /// @param serializer   Sample serializer instance (to serialize data)
    /// @param storage      Storage service instance (to store data)
    /// @param publisher    Data publisher instance (to report execution result)
    /// @param samplesCount Number of samples to collect
    CTestModeCommand(
        const ISampler::Ptr & sampler,
        const ISampleSerializer::Ptr & serializer,
        const IStorageService::Ptr & storage,
        const IDataPublisherService::Ptr & publisher,
        int samplesCount );

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

    ComponentConfig readConfig( const std::string & filename ) const;

    ISampler::Ptr mSampler;                 ///< Sampler instance
    ISampleSerializer::Ptr mSerializer;     ///< Sample serializer instance
    IStorageService::Ptr mStorageService;   ///< Storage service interface
    IDataPublisherService::Ptr mPublisher;  ///< Data publisher interface
    int mSamplesCount;                      ///< Number of samples to collect
};

#endif // CTESTMODECOMMAND_H