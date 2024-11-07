#ifndef CSONDEAPP_H
#define CSONDEAPP_H

#include "publisher/IDataPublisherService.h"
#include "sampler/ISampler.h"

class CSondeApp
{
public:
    CSondeApp( ISampler & sampler,
               IDataPublisherService & publisher );

    void run();

private:
    ISampler & mSampler;
    IDataPublisherService & mPublisher;

    static const char * TAG;
};


#endif // CSONDEAPP_H