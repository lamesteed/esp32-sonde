#ifndef CSETTIMECOMMAND_H
#define CSETTIMECOMMAND_H

#include "ICommand.h"
#include "IDataPublisherService.h"
#include "ITimeService.h"

/// @brief Command to set system time
class CSetTimeCommand : public ICommand
{
public:
    /// @brief Constructor
    /// @param publisher    data publisher interface (to report execution result)
    /// @param timeService  time service interface
    /// @param args         key-value pairs of command arguments (expected key: "ts")
    CSetTimeCommand(
        const IDataPublisherService::Ptr & publisher,
        const ITimeService::Ptr & timeService,
        const ICommand::CommandArgs & args );

    /// @brief Virtual destructor
    virtual ~CSetTimeCommand() = default;

private:
    // ICommand interface
    /// @brief Get command description
    /// @return Human firendly description of the command
    virtual std::string getDescription() const override;

    /// @brief Execute the command
    /// @return execution result, true if command executed successfully, false otherwise
    virtual bool execute() override;

private:
    static const char * ARG_TIMESTAMP;       ///< Name of the argument that specifies unix timestamp in milliseconds

    IDataPublisherService::Ptr mPublisher;  ///< Data publisher interface
    ITimeService::Ptr mTimeService;         ///< Time service interface
    ICommand::CommandArgs mArgs;            ///< Command arguments
};

#endif // CSETTIMECOMMAND_H