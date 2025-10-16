#ifndef LOW_POWER_CONFIG_H
#define LOW_POWER_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

    void vPortSetupTimerInterrupt(void);
    void RTC0_IRQHandler(void);
    void vApplicationIdleHook(void);

#ifdef __cplusplus
}
#endif

#endif // LOW_POWER_CONFIG_H