#ifndef ICOMMAND_H
#define ICOMMAND_H

#include <string>

class ICommand {
public:
    virtual ~ICommand() {}
    virtual std::string getDescription() const = 0;
    virtual bool execute() = 0;
};

#endif // ICOMMAND_H