#ifndef CFIELDMODECOMMAND_H
#define CFIELDMODECOMMAND_H

#include "ICommand.h"
#include "ISampler.h"
#include "ISampleSerializer.h"
#include "IStorageService.h"
#include "IDataPublisherService.h"
#include "IWatchdog.h"
#include "ComponentConfig.h"


class CFieldModeCommand : public ICommand
{
public:

    using SampleDataList = std::list<SampleData::Ptr>;
    using SampleDataListPtr = std::shared_ptr<SampleDataList>;
    using SamplingResultsByDepth = std::map< float, SampleDataList>;

    /// @brief Constructor
    /// @param sampler Sampler instance
    /// @param serializer Sample serializer instance
    /// @param storage Storage service instance
    /// @param publisher Data publisher instance
    /// @param watchdog Watchdog instance
    /// @param args Command arguments (expected key: "filename")
    CFieldModeCommand(
        const ISampler::Ptr & sampler,
        const ISampleSerializer::Ptr & serializer,
        const IStorageService::Ptr & storage,
        const IDataPublisherService::Ptr & publisher,
        const IWatchdog::Ptr & watchdog,
        const ICommand::CommandArgs & args
    );

    /// @brief Virtual destructor
    virtual ~CFieldModeCommand() = default;

private:
    // ICommand interface
    /// @brief Get command description
    /// @return Human firendly description of the command
    virtual std::string getDescription() const override;

    /// @brief Execute the command
    /// @return execution result, true if command executed successfully, false otherwise
    virtual bool execute() override;

private:

    void addSampleToDepthBucket( SamplingResultsByDepth & depthBuckets, const SampleData::Ptr & sample, float depthInterval ) const;

    SampleDataListPtr buildAverageSamplingByDepth( const SamplingResultsByDepth & sampleResults ) const;

    void storeSample( const SampleData::Ptr & sample, const std::string & filename, bool append );

    void storeSamples( const SampleDataListPtr & samples, const std::string & filename );

    ComponentConfig readConfig( const std::string & filename ) const;

    static const char * ARG_FILENAME;       ///< Name of the argument that specifies file name
    static const char * TAG;                ///< Tag for logging

    ISampler::Ptr mSampler;                 ///< Sampler instance
    ISampleSerializer::Ptr mSerializer;     ///< Sample serializer instance
    IStorageService::Ptr mStorageService;   ///< Storage service interface
    IDataPublisherService::Ptr mPublisher;  ///< Data publisher interface
    IWatchdog::Ptr mWatchdog;               ///< Watchdog instance
    ICommand::CommandArgs mArgs;            ///< Command arguments
};

#endif // CFIELDMODECOMMAND_H