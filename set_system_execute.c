#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "updateValue.h"
#include "set_system_execute.h"
#include "sdkconfig.h"

#define USDS_PREV_MASK (1 << 0)
#define USDS_CURR_MASK (1 << 1)
#define BED_PREV_MASK (1 << 2)
#define BED_CURR_MASK (1 << 3)
#define BED_LENGTH 200 // 2m

 int bed_state;
 int usds_state;
 int system_state;
 int outPut; //0=nothing:1=low:2=high

 int bed_prev;
 int bed_current;
 int usds_prev;
 int usds_current;

 bool onOffSwitch;
 bool key;
 bool cancelBotton;


void set_execute(void)
{
    set_bed_current();
    set_usds_current();
    set_system();
    execute_action(onOffSwitch, key, cancelBotton);

    return;
}
// Set Current Bed State
int set_bed_current(void)
{

    int fsr1_state;
    int fsr2_state;

    fsr1_state = ((fsr1Value < 0.6 * fsr1MaxValue) ? 0 : 1);

    fsr2_state = ((fsr2Value < 0.6 * fsr2MaxValue) ? 0 : 1);

    bed_state = ((fsr1_state == 0 && fsr2_state == 0) ? 0 : 1);

    return (bed_state);
}

// Set current usds state
int set_usds_current(void)
{
    float usds_distance = (float)utsValue/5;

    if (usds_distance <= 16) // Minimum detectable distance = 15.2cm
    {
        usds_state = 2;
    }
    else if (usds_distance <= 0.75 * BED_LENGTH)
    {
        usds_state = 1;
    }
    else
    {
        usds_state = 0;
    }
    return (usds_state);
}

void inititialStage(){
    bed_prev = set_bed_current();   // set initial bed state
    usds_prev = set_usds_current(); // set initial us state
}

void updatingStage(){

        bed_prev = bed_current;
        usds_prev = usds_current;
}

void set_system(void)
{
    int state_temp = 0;

    if (bed_prev == 1)
    {
        state_temp |= BED_PREV_MASK;
    }
    else
    {
        state_temp &= ~BED_PREV_MASK;
    }

    if (usds_prev == 1)
    {
        state_temp |= USDS_PREV_MASK;
    }
    else
    {
        state_temp &= ~USDS_PREV_MASK;
    }

    if (bed_current == 1)
    {
        state_temp |= BED_CURR_MASK;
    }
    else
    {
        state_temp &= ~BED_CURR_MASK;
    }

    if (usds_current == 1)
    {
        state_temp |= BED_CURR_MASK;
    }
    else
    {
        state_temp &= ~BED_CURR_MASK;
    }

    // if-else - truth table conditional program
    // if (state_temp & (BED_CURR_MASK | BED_PREV_MASK | USDS_CURR_MASK | USDS_PREV_MASK) == 0) // system = [ 0 0 0 0 ]
    // {
    //     system_state = 0; // "reset_Default"; // no sensors triggered. Default case.
    //     printf("System State: Reset Default");
    // }
    // else if (state_temp & (BED_CURR_MASK | BED_PREV_MASK | USDS_CURR_MASK | USDS_PREV_MASK) == 3) // system = [0 0 1 1 ]
    // {
    //     system_state = 1; //"PUE"; // usds triggered but system empty. 'PUE' = Possible User Error
    // }
    // else if (state_temp & (BED_CURR_MASK | BED_PREV_MASK | USDS_CURR_MASK | USDS_PREV_MASK) == 4) // system = [ 0 1 0 0 ]
    // {
    //     system_state = 2; // "fall_Out"; // system vacated, no change in usds
    // }
    // else if (state_temp & (BED_CURR_MASK | BED_PREV_MASK | USDS_CURR_MASK | USDS_PREV_MASK) == 7) // system = [ 0 1 1 1 ]
    // {
    //     system_state = 3; // "sit_Exit";
    // }
    // else if (state_temp & (BED_CURR_MASK | BED_PREV_MASK | USDS_CURR_MASK | USDS_PREV_MASK) == 13) // system = [ 1 1 0 1 ]
    // {
    //     system_state = 4; //"bed_Occupied";
    // }
    // else if (state_temp & (BED_CURR_MASK | BED_PREV_MASK | USDS_CURR_MASK | USDS_PREV_MASK) == 11) // system = [ 1 0 1 1 ]
    // {
    //     system_state = 5; //"bed_Enter";
    // }
    // else if (state_temp & (BED_CURR_MASK | BED_PREV_MASK | USDS_CURR_MASK | USDS_PREV_MASK) == 15) // system = [ 1 1 1 1 ]
    // {
    //     system_state = 6; //"sit_Warning";
    // }
    // else if ((usds_current == 2) | (usds_prev == 2))
    // {
    //     system_state = 7 // "usds_Covered";
    // }
    // else
    // {
    //     system_state = 8; // "error";
    // }

    int state = (state_temp & 15); // 15 = [ 1 1 1 1 ] = (BED_CURR_MASK | BED_PREV_MASK | USDS_CURR_MASK | USDS_PREV_MASK)
    switch (state)
    {
    case 0:
        system_state = 0; // "reset_Default"; // no sensors triggered. Default case.
        printf("System State: Reset Default");
        break;

    case 3:
        system_state = 0; //"PUE"; // usds triggered but system empty. 'PUE' = Possible User Error
        break;

    case 4:
        system_state = 2; // "fall_Out"; // system vacated, no change in usds // high priority
        break;

    case 7:
        system_state = 2; // "sit_Exit"; // high priority
        break;

    case 11:
        system_state = 0; //"bed_Enter";
        break;

    case 13:
        system_state = 0; //"bed_Occupied";
        break;

    case 15:
        system_state = 1; //"sit_Warning"; // low priority
        break;

    default:
        system_state = 0; // error
    }
    return;
}

// void execute_action(bool onOffSwitch, bool key,bool cancelBotton)
// {

//     //switch-case - action based on system state
//     if (onOffSwitch == true && key == true){

//     switch (system_state)
//     {
//     case 0: //"reset_Default":
//         printf("Reset default\n");
//         break;
//     case 1: //"PUE":
//         printf("Possible User Error\n");
//         break;
//     case 2: //"fall_Out":
//         printf("Emergency Warning: Fall Out\n");
//         // gpio_set_level(gpio_num_tgpio_num, uint32_t level)
//         break;
//     case 3: //"sit_Exit":
//         printf("Warning: Sit-Up Exit\n");
//         // gpio_set_level(gpio_num_tgpio_num, uint32_t level)
//         break;
//     case 4: //"bed_Occupied":
//         printf("Bed Occupied\n");
//         // gpio_set_level(gpio_num_tgpio_num, uint32_t level)
//         break;
//     case 5: //"bed_Enter" :
//         printf("Bed Entered\n");
//         // gpio_set_level(gpio_num_tgpio_num, uint32_t level)
//         break;
//     case 6: //"sit_Warning":
//         printf("Warning: Sit-Up, Possible Exit");
//         // gpio_set_level(gpio_num_tgpio_num, uint32_t level)
//         break;
//     case 7: //"usds_Covered":
//         printf("Distance Sensor Covered");
//         // gpio_set_level(gpio_num_tgpio_num, uint32_t level)
//         break;
//     case 8: //"error":
//         printf("System State Error");
//         break;
//     }
//     }
//     return;
// }

void execute_action(bool onOffSwitch, bool key, bool cancelBotton)
{

    //switch-case - action based on system state
    if (onOffSwitch == true && cancelBotton == false)
    {

        switch (system_state)
        {
        case 0: //"reset_Default":
            outPut = 0;
            printf("no problem\n");
            break;
        case 1: //"PUE":
            outPut = 1;
            printf("low priority\n");
            break;
        case 2: //"fall_Out":
            outPut = 2;
            printf("high priority\n");
            // gpio_set_level(gpio_num_tgpio_num, uint32_t level)
            break;
        }
    }
    else if (cancelBotton == true && key == true)
    {
        outPut = 0;
    }
    else
    {
        outPut = 0;
    }
    return;
}