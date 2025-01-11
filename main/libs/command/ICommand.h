#ifndef ICOMMAND_H
#define ICOMMAND_H

#include <memory>
#include <string>

class ICommand {
public:
    using Ptr = std::shared_ptr<ICommand>;

    virtual ~ICommand() = default;

    virtual std::string getDescription() const = 0;
    virtual bool execute() = 0;
};

#endif // ICOMMAND_H