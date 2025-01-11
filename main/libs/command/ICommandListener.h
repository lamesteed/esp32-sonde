#ifndef ICOMMANDLISTENER_H
#define ICOMMANDLISTENER_H

#include <string>

/// @brief Interface for command listener
class ICommandListener
{
public:

    /// @brief Virtual destructor
    virtual ~ICommandListener() = default;

    /// @brief A command received from the client
    /// @param command Command name
    /// @param args    Command arguments ("key1=value1,key2=value2,..." format)
    virtual void onCommandReceived( const std::string & command, const std::string & args ) = 0;
};

#endif // ICOMMANDLISTENER_H