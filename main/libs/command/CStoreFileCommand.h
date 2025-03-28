#ifndef CSTOREFILECOMMAND_H
#define CSTOREFILECOMMAND_H
#include "ICommand.h"
#include "IStorageService.h"
#include "IDataPublisherService.h"

class CStoreFileCommand : public ICommand
{
public:
    /// @brief Constructor
    /// @param storageService storage service interface
    /// @param publisher      data publisher interface
    /// @param args           key-value pairs of command arguments (expected keys: "filename", "data")
    CStoreFileCommand(
        const IStorageService::Ptr & storageService,
        const IDataPublisherService::Ptr publisher,
        const ICommand::CommandArgs & args );

    /// @brief Virtual destructor
    virtual ~CStoreFileCommand() = default;

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
    static const char * ARG_DATA;           ///< Name of the argument that specifies data to be stored in the file

    IStorageService::Ptr mStorageService;   ///< Storage service interface
    IDataPublisherService::Ptr mPublisher;  ///< Data publisher interface
    ICommand::CommandArgs mArgs;            ///< Command arguments
};


#endif // CSTOREFILECOMMAND_H