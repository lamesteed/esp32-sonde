#ifndef CREBOOTCOMMAND_H
#define CREBOOTCOMMAND_H

#include "ICommand.h"
#include "IDataPublisherService.h"
#include "IRebootable.h"

/// @brief Command to reboot the device
class CRebootCommand : public ICommand
{
public:
    /// @brief Constructor
    /// @param publisher    data publisher interface
    /// @param rebootable   rebootable interface
    CRebootCommand(
        const IDataPublisherService::Ptr & publisher,
        const IRebootable::Ptr & rebootable );

    /// @brief Virtual destructor
    virtual ~CRebootCommand() = default;

private:
    // ICommand interface

    /// @brief Get command description
    /// @return Human firendly description of the command
    virtual std::string getDescription() const override;

    /// @brief Execute the command
    /// @return execution result, true if command executed successfully, false otherwise
    virtual bool execute() override;

    IDataPublisherService::Ptr mPublisher;  ///< Data publisher interface
    IRebootable::Ptr mRebootable;           ///< Rebootable interface
};

#endif // CREBOOTCOMMAND_H