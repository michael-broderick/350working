#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "updateValue.h"
#include "sdkconfig.h"

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling

 esp_adc_cal_characteristics_t *adc_chars;

 const adc_channel_t fsr1Channel = ADC1_CHANNEL_4;            //Pin32
 const adc_channel_t fsr2Channel = ADC1_CHANNEL_5;            //Pin33
 const adc_channel_t utsChannel = ADC1_CHANNEL_6;             //PinA2

 const adc_atten_t atten = ADC_ATTEN_DB_0;
 const adc_unit_t unit = ADC_UNIT_1;

int fsr1Value;
 int fsr2Value;
 int utsValue;

 int fsr1MaxValue;
 int fsr2MaxValue;


// int calibrationValue[2];
 void print_char_val_type(esp_adc_cal_value_t val_type)
{
   
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}

 void systemInitialise(void)
{
    //Configure ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(fsr1Channel, atten);
    adc1_config_channel_atten(fsr2Channel, atten);
    adc1_config_channel_atten(utsChannel, atten);

    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);
}

 void check_efuse(void)
{
    //Check TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }
    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
}

 void fsr1Read(void)
{
    uint32_t fsr1_reading = 0;
        //Multisampling
    for (int i = 0; i < NO_OF_SAMPLES; i++) 
    {
        if (unit == ADC_UNIT_1) {
            fsr1_reading += adc1_get_raw((adc1_channel_t)fsr1Channel);
        } 
    }
    fsr1_reading /= NO_OF_SAMPLES;
    //Convert adc_reading to voltage in mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(fsr1_reading, adc_chars);
    printf("FSR1 Reading    --> Raw: %d\tVoltage: %dmV\n", fsr1_reading, voltage);
    vTaskDelay(pdMS_TO_TICKS(1000));
    fsr1Value = (int)fsr1_reading;

}
 void fsr2Read(void)
    {
        uint32_t fsr2_reading = 0;
        //Multisampling
    for (int i = 0; i < NO_OF_SAMPLES; i++) 
    {
        if (unit == ADC_UNIT_1) {
            fsr2_reading += adc1_get_raw((adc1_channel_t)fsr2Channel);
        } 
    }
    fsr2_reading /= NO_OF_SAMPLES;
    //Convert adc_reading to voltage in mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(fsr2_reading, adc_chars);
    printf("FSR2 Reading    --> Raw: %d\tVoltage: %dmV\n", fsr2_reading, voltage);
    vTaskDelay(pdMS_TO_TICKS(1000));
    fsr2Value = (int)fsr2_reading;
    }

 void utsRead(void)
    {
        uint32_t uts_reading = 0;
        //Multisampling
    for (int i = 0; i < NO_OF_SAMPLES; i++) 
    {
        if (unit == ADC_UNIT_1) {
            uts_reading += adc1_get_raw((adc1_channel_t)utsChannel);
        } 
    }
    uts_reading /= NO_OF_SAMPLES;
    //Convert adc_reading to voltage in mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(uts_reading, adc_chars);
    printf("USS reading     --> Raw: %d\tVoltage: %dmV\n", uts_reading, voltage);
    vTaskDelay(pdMS_TO_TICKS(1000));
    utsValue = (float)uts_reading;
    }



 void initialValueUpdatingFcn(void)
{

    fsr1MaxValue = fsr1Value;
    fsr2MaxValue = fsr2Value;

}

void sensor_read(void)
{

    fsr1Read();
    fsr2Read();
    utsRead();
  
}

