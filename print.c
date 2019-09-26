// Print output function

// sensor value
// sensor state
// system state
// priority
// key inserted
// cancel button

#include "print.h"

void printFunc(void)
{
    printf("================================================================\n\n");

    printf("System State:\n");
    switch (systemState)
    {
    case 0: //"reset_Default":
        printf("Reset default. No Output.\n");
        // no output
        break;
    case 1: //"PUE":
        printf("Possible User Error. No Output.\n");
        // no output
        break;
    case 2: //"fall_Out":
        printf("Emergency Warning: Fall Out. High Priority Output!\n");
        // high priority
        // gpio_set_level(gpio_num_tgpio_num, uint32_t level)
        break;
    case 3: //"sit_Exit":
        printf("Warning: Sit-Up Exit. High Priority Output!\n");
        // high priority
        // gpio_set_level(gpio_num_tgpio_num, uint32_t level)
        break;
    case 4: //"bed_Occupied":
        printf("Bed Occupied. No Output.\n");
        // no output
        // gpio_set_level(gpio_num_tgpio_num, uint32_t level)
        break;
    case 5: //"bed_Enter" :
        printf("Bed Entered. No Output.\n");
        // no output
        // gpio_set_level(gpio_num_tgpio_num, uint32_t level)
        break;
    case 6: //"sit_Warning":
        printf("Warning: Sit-Up, Possible Exit. Low Priority Output!\n");
        // low priority
        // gpio_set_level(gpio_num_tgpio_num, uint32_t level)
        break;
    case 7: //"usds_Covered":
        printf("Distance Sensor Covered. Low Priority Output!\n");
        // low priority
        // gpio_set_level(gpio_num_tgpio_num, uint32_t level)
        break;
    case 8: //"error":
        printf("System State Error. No Output.\n");
        break;
    }

    if (fsr1State == 1)
    {
        printf("FSR 1 State: 1\n");
    }
    else
    {
        printf("FSR 1 State: 0\n");
    }

    if (fsr2State == 1)
    {
        printf("FSR 2 State: 1\n");
    }
    else
    {
        printf("FSR 2 State: 0\n");
    }

    printf("FSR 1 Value: %d, FSR 1 Threshold: %d\n", fsr1Value, fsr1Threshold);
    printf("FSR 2 Value: %d, FSR 2 Threshold: %d\n\n", fsr2Value, fsr2Threshold);


    if (usdsState == 1)
    {
        printf("Ultrasonic State: 1\n\n");
    }
    else
    {
        printf("Ultrasonic State: 0\n\n");
    }

    printf("Ultrasonic Distance: %d, Distance Threshold: %d\n", usdsDistance, usdsThreshold);

    printf("================================================================\n\n");

    return;
}