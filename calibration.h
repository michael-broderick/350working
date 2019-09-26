#ifndef CALIBRATION_H_
#define CALIBRATION_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "updateValue.h"

#define GPIO_OUTPUT_IO_0 18
#define GPIO_OUTPUT_PIN_SEL (1ULL << GPIO_OUTPUT_IO_0)
#define GPIO_INPUT_IO_0 4
#define GPIO_INPUT_PIN_SEL (1ULL << GPIO_INPUT_IO_0)
#define ESP_INTR_FLAG_DEFAULT 0

 void IRAM_ATTR gpio_isr_handler(void *arg);
 void gpio_task_example(void *arg);
void calibrationInitialise(void);
 void calibrationMain(void);


#endif