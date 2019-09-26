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
#include "setSystem_execute.h"
#include "print.h"


void app_main(void)
{
    checkEfuse();                   // ESP reference voltage configure for ADC
    systemInitialise();             // ADC configuration
    calibrationInitialise();        // Calibration/interrupt configuration
    
    sensorRead();                   // initial read sensor values
    int bedPrev = setBedCurrent();      // set initial bed state
    int usdsPrev = setUsdsCurrent(); // set initial us state


    while (1)
    {
        calibrationMain();          // check for calibration to set thresholds
        sensorRead();               // Read sensors
        int fsr1Value = fsr1Read();
        int fsr2Value = fsr1Read();

        setExecute();               // set current sensor states
        printFunc();                // print output: warning, system state, sensor state, sensor value, sensor threshold
        bedPrev = bedCurrent; // update previous state for next loop in main file
        usdsPrev = usdsCurrent;
        vTaskDelay(1000 / portTICK_RATE_MS);    // delay and loop
    }
}