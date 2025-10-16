#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include "low_power_config.h"

extern "C" void vPortSetupTimerInterrupt(void)
{
    // Enable LFCLK if not already enabled
    NRF_CLOCK->LFCLKSRC = CLOCK_LFCLKSRC_SRC_Xtal; // Use external crystal
    NRF_CLOCK->TASKS_LFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
    {
    }
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;

    // Configure RTC0
    NRF_RTC0->PRESCALER = 327;                      // Set prescaler for 10 ms per tick (100 Hz)
    NRF_RTC0->CC[0] = 100;                          // Set compare register for the interval, 100 ticks (100 tick where a tick is 10 ms = every 1 s)
    NRF_RTC0->INTENSET = RTC_INTENSET_COMPARE0_Msk; // Enable interrupt on COMPARE0
    NRF_RTC0->TASKS_START = 1;                      // Start the RTC

    // Enable RTC0 interrupt in NVIC
    NVIC_EnableIRQ(RTC0_IRQn);
}

extern "C" void RTC0_IRQHandler(void)
{
    if (NRF_RTC0->EVENTS_COMPARE[0] != 0)
    {
        NRF_RTC0->EVENTS_COMPARE[0] = 0; // Clear the interrupt
        NRF_RTC0->TASKS_CLEAR = 1;       // Reset the RTC counter

        BaseType_t xSwitchRequired = xTaskIncrementTick();
        portYIELD_FROM_ISR(xSwitchRequired);
    }
}

extern "C" void vApplicationIdleHook(void)
{
    __WFE(); // Enter low-power mode until the next event or interrupt
}
