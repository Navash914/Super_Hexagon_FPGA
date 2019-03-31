#ifndef MAIN_C_CONFIG_GIC_H
#define MAIN_C_CONFIG_GIC_H

#include "address_map_arm.h"
#include "defines.h"

void config_gic();
void enable_interrupt(int interrupt_id);

#endif //MAIN_C_CONFIG_GIC_H
