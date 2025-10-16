#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <semphr.h>
#include "low_power_config.h" //Preconfig for low power in vTaskDelay if there is no other task to run.
#include "vars/vektor.h"
#include "vars/fun_led.h"
#include "vars/batStatus.h"
#include "vars/Settings.h"
#include "vars/ctl_tag_commun.h"
#include "vars/tasks.h"
#include <SPI.h>      //the LoRa device is SPI based so load the SPI library
#include <SX128XLT.h> //include the appropriate library

void setup()
{
  led_Init();
  bat_status_init();
  SPI.setPins(MISO, SCK, MOSI);
  SPI.begin();
  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, LORA_DEVICE) != true)
  {
    while (1)
      led_Flash(10, 50); // If failed to connect to SX1280, led flashes
  }
  randomSeed(NetworkID << 8 | Node_address); // To create unique randomness among different nodes
  LT.setupLoRa(communFreq, 0, communSF, communBW, communCR);
  LT.setCadParam(LORA_CAD_08_SYMBOL); // channel activity detection setup
  sleepStatus = false;
  xBlock_AOn_Rang_sem = xSemaphoreCreateBinary();
  xSemaphoreGive(xBlock_AOn_Rang_sem);
  xBlock_LP_Commun_sem = xSemaphoreCreateBinary();
  xSemaphoreGive(xBlock_LP_Commun_sem);
  TaskListen_TimerHandle = xTimerCreate("Listener Timer", 3, pdFALSE, (void *)0, TaskListen_CallbackTimer); // Timer initiation (3 tick (3 seconds here)), non repeat
  xTaskCreate(TaskCfgCheck, "Cfg Check", 1024, NULL, 4, NULL);
  vTaskStartScheduler(); // Start the scheduler
}

void loop()
{
  // No code here, as FreeRTOS handles the execution flow.
}
