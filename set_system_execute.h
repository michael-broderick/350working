#ifndef SET_SYSTEM_EXECUTE_H_
#define SET_SYSTEM_EXECUTE_H_

#include "updateValue.h"
#include "sdkconfig.h"

#define USDS_PREV_MASK (1 << 0)
#define USDS_CURR_MASK (1 << 1)
#define BED_PREV_MASK (1 << 2)
#define BED_CURR_MASK (1 << 3)
#define BED_LENGTH 200 // 2m

void set_execute(void);
int set_bed_current(void);
int set_usds_current(void);
void inititialStage(void);
void updatingStage(void);
void set_system(void);
void execute_action(bool onOffSwitch, bool key, bool cancelBotton);

#endif