#ifndef ICOMMAND_H
#define ICOMMAND_H

#include <memory>
#include <string>
#include <map>

/// @brief Generic command interface
class ICommand {
public:
    using Ptr = std::shared_ptr<ICommand>;
    using CommandArgs = std::map<std::string, std::string>;

    /// @brief Virtual destructor
    virtual ~ICommand() = default;

    /// @brief Get command description
    /// @return Human firendly description of the command
    virtual std::string getDescription() const = 0;

    /// @brief Execute the command
    /// @return execution result, true if command executed successfully, false otherwise
    virtual bool execute() = 0;
};

#endif // ICOMMAND_H