#ifndef CLISTFILESCOMMAND_H
#define CLISTFILESCOMMAND_H

#include "ICommand.h"
#include "IStorageService.h"
#include "IDataPublisherService.h"

/// @brief Command to list available files in Sentry226 Sonde storage
class CListFilesCommand : public ICommand
{
public:
    /// @brief Constructor
    /// @param storageService storage interface
    /// @param publisher      data publisher interface
    CListFilesCommand(
        const IStorageService::Ptr & storageService,
        const IDataPublisherService::Ptr publisher );

    /// @brief Virtual destructor
    virtual ~CListFilesCommand() = default;

private:
    // ICommand interface

    /// @brief Get command description
    /// @return Human firendly description of the command
    virtual std::string getDescription() const override;

    /// @brief Execute the command
    /// @return execution result, true if command executed successfully, false otherwise
    virtual bool execute() override;

private:
    IStorageService::Ptr mStorageService;   ///< Storage service interface
    IDataPublisherService::Ptr mPublisher;  ///< Data publisher interface

};

#endif // CLISTFILESCOMMAND_H

