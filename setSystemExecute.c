#include "setSystemExecute.h"

#define USDS_PREV_MASK (1 << 0) // [ 0 0 0 1 ] = 1
#define USDS_CURR_MASK (1 << 1) // [ 0 0 1 0 ] = 2
#define BED_PREV_MASK (1 << 2)  // [ 0 1 0 0 ] = 4
#define BED_CURR_MASK (1 << 3)  // [ 1 0 0 0 ] = 8
#define BED_LENGTH 200          // 2m

void setExecute(int fsr1Value, int fsr2Value, int fsr1MaxValue, int fsr2MaxValue, float usdsValue, float sitUpTriggerDistance, int bedPrev, int usdsPrev, int bedCurrent, int usdsCurrent, bool onOffSwitch, bool key, bool cancelButton, int systemState) // function to be called from main file to set state and execute action required
{
    setBedCurrent( fsr1Value, fsr2Value, fsr1MaxValue, fsr2MaxValue );
    setUsdsCurrent( usdsValue, sitUpTriggerDistance );
    setSystem( bedPrev, usdsPrev, bedCurrent, usdsCurrent );
    executeAction( onOffSwitch, key, cancelButton, systemState );

    return;
}
// Set Current Bed State
int setBedCurrent(int fsr1Value,int fsr2Value,int fsr1MaxValue,int fsr2MaxValue)
{

    int fsr1State;
    int fsr2State;
    int bedState;

    fsr1State = ((fsr1Value < 0.6 * fsr1MaxValue) ? 0 : 1); // fsrvalue set from fsr read, fsrmaxvalue from calibration

    fsr2State = ((fsr2Value < 0.6 * fsr2MaxValue) ? 0 : 1); // fsrvalue set from fsr read, fsrmaxvalue from calibration

    bedState = ((fsr1State == 0 && fsr2State == 0) ? 0 : 1); // a positive state of either fsr will set bed high

    return (bedState);
}

// Set current usds state
int setUsdsCurrent(float usdsValue, float sitUpTriggerDistance)
{
    float usdsDistance = usdsValue / 5; // arbitrary equation to be modified during testing
    int usdsState;

    if (usdsDistance <= 16) // Minimum detectable distance = 15.2cm
    {
        usdsState = 2; // 'covered' state
    }
    else if (usdsDistance <= sitUpTriggerDistance) // sitUpTriggerDistance to be set in calibration
    {
        usdsState = 1; // sitting up
    }
    else
    {
        usdsState = 0; // untriggered
    }
    return (usdsState);
}

int setSystem(int bedPrev,int usdsPrev,int bedCurrent,int usdsCurrent)
{
    int stateTemp = 0;
    int systemState;

    if (bedPrev == 1)
    {
        stateTemp |= BED_PREV_MASK; // stateTemp | [ 0 1 0 0 ]
    }
    else
    {
        stateTemp &= ~BED_PREV_MASK; // stateTemp & [ 1 0 1 1 ]
    }

    if (usdsPrev == 1)
    {
        stateTemp |= USDS_PREV_MASK; // stateTemp | [ 0 0 0 1 ]
    }
    else
    {
        stateTemp &= ~USDS_PREV_MASK; // stateTemp & [ 1 1 1 0 ]
    }

    if (bedCurrent == 1)
    {
        stateTemp |= BED_CURR_MASK; // stateTemp | [ 1 0 0 0 ]
    }
    else
    {
        stateTemp &= ~BED_CURR_MASK; // stateTemp & [ 0 1 1 1 ]
    }

    if (usdsCurrent == 1)
    {
        stateTemp |= USDS_CURR_MASK; // stateTemp | [ 0 0 1 0 ]
    }
    else
    {
        stateTemp &= ~USDS_CURR_MASK; // stateTemp & [ 1 1 0 1 ]
    }

    // if-else - truth table conditional program
    if (stateTemp & (BED_CURR_MASK | BED_PREV_MASK | USDS_CURR_MASK | USDS_PREV_MASK) == 0) // system = [ 0 0 0 0 ]
    {
        systemState = 0; // "reset_Default"; // no sensors triggered. Default case.
        printf("System State: Reset Default");
    }
    else if (stateTemp & (BED_CURR_MASK | BED_PREV_MASK | USDS_CURR_MASK | USDS_PREV_MASK) == 3) // system = [0 0 1 1 ]
    {
        systemState = 1; //"PUE"; // usds triggered but system empty. 'PUE' = Possible User Error
    }
    else if (stateTemp & (BED_CURR_MASK | BED_PREV_MASK | USDS_CURR_MASK | USDS_PREV_MASK) == 4) // system = [ 0 1 0 0 ]
    {
        systemState = 2; // "fall_Out"; // system vacated, no change in usds
    }
    else if (stateTemp & (BED_CURR_MASK | BED_PREV_MASK | USDS_CURR_MASK | USDS_PREV_MASK) == 7) // system = [ 0 1 1 1 ]
    {
        systemState = 3; // "sit_Exit";
    }
    else if (stateTemp & (BED_CURR_MASK | BED_PREV_MASK | USDS_CURR_MASK | USDS_PREV_MASK) == 13) // system = [ 1 1 0 1 ]
    {
        systemState = 4; //"bed_Occupied";
    }
    else if (stateTemp & (BED_CURR_MASK | BED_PREV_MASK | USDS_CURR_MASK | USDS_PREV_MASK) == 11) // system = [ 1 0 1 1 ]
    {
        systemState = 5; //"bed_Enter";
    }
    else if (stateTemp & (BED_CURR_MASK | BED_PREV_MASK | USDS_CURR_MASK | USDS_PREV_MASK) == 15) // system = [ 1 1 1 1 ]
    {
        systemState = 6; //"sit_Warning";
    }
    else if ((usdsCurrent == 2) | (usdsPrev == 2))
    {
        systemState = 7 // "usds_Covered";
    }
    else
    {
        systemState = 8; // "error";
    }

    return(systemState);
}
    //     int state = (stateTemp & 15); // 15 = [ 1 1 1 1 ] = (BED_CURR_MASK | BED_PREV_MASK | USDS_CURR_MASK | USDS_PREV_MASK)
    //     switch (state)
    //     {
    //     case 0:
    //         systemState = 0; // "reset_Default"; // no sensors triggered. Default case.
    //         printf("System State: Reset Default");
    //         break;

    //     case 3:
    //         systemState = 0; //"PUE"; // usds triggered but system empty. 'PUE' = Possible User Error
    //         break;

    //     case 4:
    //         systemState = 2; // "fall_Out"; // system vacated, no change in usds // high priority
    //         break;

    //     case 7:
    //         systemState = 2; // "sit_Exit"; // high priority
    //         break;

    //     case 11:
    //         systemState = 0; //"bed_Enter";
    //         break;

    //     case 13:
    //         systemState = 0; //"bed_Occupied";
    //         break;

    //     case 15:
    //         systemState = 1; //"sit_Warning"; // low priority
    //         break;

    //     default:
    //         systemState = 0; // error
    //     }
    //     return;
    // }

int executeAction(bool onOffSwitch, bool key, bool cancelButton, int systemState)
{
    // pins for bool variables need to be set here
    bool onOffSwitch;  // used to control output action
    bool key;          // used to control output action
    bool cancelButton; // check cancel button before output

    int outPut;

    //switch-case - action based on system state
    if (onOffSwitch == true && cancelButton == false)
    {

            switch (systemState)
            {
            case 0: //"reset_Default":
                outPut = 0;     // no output
                break;
            case 1: //"PUE":
                outPut = 0;     // no output
                break;
            case 2: //"fall_Out":
                outPut = 2;     // no output
                // gpio_set_level(gpio_num_tgpio_num, uint32_t level)
                break;
            case 3: //"sit_Exit":
                outPut = 2;     // no output
                // gpio_set_level(gpio_num_tgpio_num, uint32_t level)
                break;
            case 4: //"bed_Occupied":
                outPut = 0;     // no output
                // gpio_set_level(gpio_num_tgpio_num, uint32_t level)
                break;
            case 5: //"bed_Enter" :
                outPut = 0;     // no output
                // gpio_set_level(gpio_num_tgpio_num, uint32_t level)
                break;
            case 6: //"sit_Warning":
                outPut = 1;     // no output
                // gpio_set_level(gpio_num_tgpio_num, uint32_t level)
                break;
            case 7: //"usds_Covered":
                outPut = 1;     // no output
                // gpio_set_level(gpio_num_tgpio_num, uint32_t level)
                break;
            case 8: //"error":
                outPut = 0;     // no output
                break;
            }
        }
        else if (cancelButton == true && key == true)
        {
            outPut = 0;
        }

        return(outPut);
    }
