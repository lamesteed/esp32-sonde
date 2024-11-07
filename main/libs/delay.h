#ifndef DELAY_H
#define DELAY_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define delayMsec(x) vTaskDelay((x) / portTICK_PERIOD_MS)


#endif // DELAY_H