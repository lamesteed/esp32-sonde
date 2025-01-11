#ifndef CGETFILECOMMAND_H
#define CGETFILECOMMAND_H

#include "ICommand.h"
#include "IStorageService.h"
#include "IDataPublisherService.h"

/// @brief Command to get specified file from Sentry226 Sonde storage
class CGetFileCommand : public ICommand
{
public:
    /// @brief Constructor
    /// @param storageService storage service interface
    /// @param publisher      data publisher interface
    /// @param args           key-value pairs of command arguments (expected key: "filename")
    CGetFileCommand(
        const IStorageService::Ptr & storageService,
        const IDataPublisherService::Ptr publisher,
        const ICommand::CommandArgs & args );

    /// @brief Virtual destructor
    virtual ~CGetFileCommand() = default;

private:
    // ICommand interface

    /// @brief Get command description
    /// @return Human firendly description of the command
    virtual std::string getDescription() const override;

    /// @brief Execute the command
    /// @return execution result, true if command executed successfully, false otherwise
    virtual bool execute() override;

private:
    static const char * ARG_FILENAME;       ///< Name of the argument that specifies file name

    IStorageService::Ptr mStorageService;   ///< Storage service interface
    IDataPublisherService::Ptr mPublisher;  ///< Data publisher interface
    ICommand::CommandArgs mArgs;            ///< Command arguments
};

#endif // CGETFILECOMMAND_H