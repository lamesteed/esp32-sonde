#ifndef IREBOOTABLE_H
#define IREBOOTABLE_H

class IRebootable
{
public:
    virtual ~IRebootable() {}
    virtual void reboot() = 0;
};

#endif // IREBOOTABLE_H