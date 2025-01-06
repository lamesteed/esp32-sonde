#ifndef CSONDEAPP_H
#define CSONDEAPP_H

#include "IRebootable.h"

class CSondeApp : public IRebootable
{
public:
    CSondeApp();

    void run();

private:
    static const char * TAG;

    // IRebootable interface
    virtual void reboot() override;

    bool mRebootPending;
};

#endif // CSONDEAPP_H