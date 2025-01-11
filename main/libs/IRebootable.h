#ifndef IREBOOTABLE_H
#define IREBOOTABLE_H

#include <memory>

class IRebootable : public std::enable_shared_from_this<IRebootable>
{
public:
    using Ptr = std::shared_ptr<IRebootable>;

    virtual ~IRebootable() = default;
    virtual void reboot() = 0;
};

#endif // IREBOOTABLE_H