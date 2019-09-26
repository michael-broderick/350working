#ifndef SETSYSTEMEXECUTE_H_
#define SETSYSTEMEXECUTE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "updateValue.h"            // uses variables from sensor reading
#include "sdkconfig.h"

#define USDS_PREV_MASK (1 << 0)
#define USDS_CURR_MASK (1 << 1)
#define BED_PREV_MASK (1 << 2)
#define BED_CURR_MASK (1 << 3)
#define BED_LENGTH 200 // 2m

void setExecute(void);
int setBedCurrent(void);
int setUsdsCurrent(void);
void setSystem(void);
void executeAction(bool onOffSwitch, bool key, bool cancelButton);

#endif