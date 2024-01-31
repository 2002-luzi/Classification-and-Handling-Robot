#include "watchDog.h"

wdog_config_t wdog_config;

void Wdog_init(void)
{
    WDOG_GetDefaultConfig(& wdog_config);
    wdog_config.timeoutValue = 0x0U;     //Timeout value is (0x0 + 1)/2 = 0.5 sec. 
    WDOG_Init(wdog_base, & wdog_config);
}