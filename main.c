#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/dac.h"
#include "esp_adc_cal.h"

#include "time.h"
#include "sdkconfig.h"
#include "calibration.h"
#include "updateValue.h"
#include "set_system_execute.h"

#define DEFAULT_VREF 1100 //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES 64  //Multisampling
// Masks used to set system state
#define USDS_PREV_MASK (1 << 0)
#define USDS_CURR_MASK (1 << 1)
#define BED_PREV_MASK (1 << 2)
#define BED_CURR_MASK (1 << 3)
#define BED_LENGTH 200 // 2m

static esp_adc_cal_characteristics_t *adc_chars;

static const adc_channel_t fsr1Channel = ADC1_CHANNEL_4;        //Pin32
static const adc_channel_t fsr2Channel = ADC1_CHANNEL_5;        //Pin33
static const adc_channel_t utsChannel = ADC1_CHANNEL_6;         //PinA2
static const adc_channel_t calibrationChannel = ADC1_CHANNEL_3; //PinA3

static const adc_atten_t atten = ADC_ATTEN_DB_0;
static const adc_unit_t unit = ADC_UNIT_1;



void app_main(void)
{
    check_efuse();
    systemInitialise();
    calibrationInitialise();
    
    sensor_read();                  // get initial bed and us values
    inititialStage();

    while (1)
    {
        calibrationMain();
        sensor_read();
        set_execute();
        updatingStage();

        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}