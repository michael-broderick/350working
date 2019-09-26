#ifndef UPDATEVALUE_H_
#define UPDATEVALUE_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling

extern int fsr1Value;
extern int fsr2Value;
extern int fsr1MaxValue;
extern int fsr2MaxValue;
extern int utsValue;

 void print_char_val_type(esp_adc_cal_value_t val_type);
 void systemInitialise(void);
 void check_efuse(void);
 void fsr1Read(void);
 void fsr2Read(void);
 void utsRead(void);
 void initialValueUpdatingFcn(void);
void sensor_read(void);

#endif