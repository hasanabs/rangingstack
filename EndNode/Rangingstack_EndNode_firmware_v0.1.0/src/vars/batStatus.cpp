#include <Arduino.h>
#include "batStatus.h"

#define Bat_READ_Power 27 // Source of power pin (on only in reading)
// Based on PPK2 Measurement:
#define ADC_MIN 12401 // 2.725 v in 2^14
#define ADC_MAX 13993 // 3.075 v in 2^14

#define ADC_RANGE (ADC_MAX - ADC_MIN)
#define SCALE_FACTOR ((255 << 6) / ADC_RANGE) // Precompute scaling factor (2^(8+6))
/*
In the real scenario (from high to low voltage because hysteresis exist)
*/

uint8_t bat_level = 0;

void bat_status_init(void)
{
    pinMode(Bat_READ_Power, OUTPUT);
    NRF_SAADC->ENABLE = (SAADC_ENABLE_ENABLE_Enabled << SAADC_ENABLE_ENABLE_Pos); // Enable SAADC
    // Configure channel 0 for single-ended input on AIN5 with internal reference
    NRF_SAADC->CH[0].PSELP = SAADC_CH_PSELP_PSELP_AnalogInput5; // Positive input: AIN5
    NRF_SAADC->CH[0].PSELN = SAADC_CH_PSELN_PSELN_NC;           // Negative input: Not connected
    NRF_SAADC->CH[0].CONFIG =
        (SAADC_CH_CONFIG_REFSEL_Internal << SAADC_CH_CONFIG_REFSEL_Pos) | // Internal reference (0.6V)
        (SAADC_CH_CONFIG_GAIN_Gain1_6 << SAADC_CH_CONFIG_GAIN_Pos) |      // Gain = 1/6
        (SAADC_CH_CONFIG_TACQ_10us << SAADC_CH_CONFIG_TACQ_Pos) |         // Acquisition time = 10 µs
        (SAADC_CH_CONFIG_MODE_SE << SAADC_CH_CONFIG_MODE_Pos) |           // Single-ended mode
        (SAADC_CH_CONFIG_BURST_Enabled << SAADC_CH_CONFIG_BURST_Pos);     // Enable burst mode
    NRF_SAADC->RESOLUTION = SAADC_RESOLUTION_VAL_14bit;
    NRF_SAADC->OVERSAMPLE = SAADC_OVERSAMPLE_OVERSAMPLE_Over16x; // Enable Oversample by 16x
    // Configure result buffer
    static int16_t result_buffer[1];
    NRF_SAADC->RESULT.PTR = (uint32_t)result_buffer;
    NRF_SAADC->RESULT.MAXCNT = 1;                 // One sample
    NRF_SAADC->INTENSET = SAADC_INTENSET_END_Msk; // Enable interrupts (Use for detecting EVENTS_END)
}

uint16_t saadc_sample(void)
{
    NRF_SAADC->TASKS_START = 1;    // Start the SAADC
    NRF_SAADC->TASKS_SAMPLE = 1;   // Trigger sampling
    while (!NRF_SAADC->EVENTS_END) // Wait for conversion to complete
    {
        // Wait for END event
    }
    NRF_SAADC->EVENTS_END = 0; // Clear the END event
    return *(uint16_t *)NRF_SAADC->RESULT.PTR;
}

uint8_t scale_adc_to_8bit(uint16_t adc_value)
{
    if (adc_value < ADC_MIN)
    {
        return 0;
    }
    if (adc_value > ADC_MAX)
    {
        return 255;
    }
    uint16_t adjusted_value = adc_value - ADC_MIN; // Adjust to start from 0
    return (adjusted_value * SCALE_FACTOR) >> 6;   // Scale and normalize
}

uint8_t read_bat_status()
{

    digitalWrite(Bat_READ_Power, HIGH);
    delay(1);
    uint8_t batStatus = scale_adc_to_8bit(saadc_sample());
    digitalWrite(Bat_READ_Power, HIGH);
    return batStatus;
}