#ifndef CSONDEAPP_H
#define CSONDEAPP_H

#include "IRebootable.h"

class CSondeApp : public IRebootable
{
public:
    CSondeApp();

    void run();

private:
    // IRebootable interface
    virtual void reboot() override;

private:
    static const char * TAG;

    bool mRebootPending;
};

#endif // CSONDEAPP_H