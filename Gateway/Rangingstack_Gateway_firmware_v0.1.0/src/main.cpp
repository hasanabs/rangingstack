#include "Settings.h"
#include "identifier.h"
#include "drivers/network.h"
#include "drivers/led_driver.h"
#include "drivers/lora_driver.h"
#include "tasks/mqtt_task.h"
#include "tasks/isr_lora.h"
#include "tasks/lora_tasks.h"
#include "utils/debug_utils.h"

void setup()
{
  // Initializing pheriperals and functionalities
  led_Init();
  IRQ_init();
  Serial.begin(115200);
  debugPrintln(F("Gateway Ranging Stack"));
  LoRa_init();
  Network_init();
  StaticTimer_init();
  LoRa_module_accessed_sem = xSemaphoreCreateMutex();

  // Starting tasks on specific core of the MCU
  xTaskCreatePinnedToCore(mqttKeeper, "MQTT keeper", 2048, NULL, 2, NULL, 0);                       // priority 2 on core 1 without handler
  xTaskCreatePinnedToCore(checkConnection, "Connection checker", 2048, NULL, 3, NULL, 0);           // priority 3 on core 1 without handler
  xTaskCreatePinnedToCore(onReceived, "LoRaTask", 4096, NULL, 1, &onReceivedHandle, 1);             // Create the task that will handle LoRa Received IRQs
  xTaskCreatePinnedToCore(answerResponse, "RespHandler", 2048, NULL, 5, &responseHandlerHandle, 1); // priority 5 on core 2 (5 is the highest priority)
  openReceiveWindow();
}

void loop()
{
}
