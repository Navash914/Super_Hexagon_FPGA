#ifndef IRQ_HANDLER_H
#define IRQ_HANDLER_H

#include <string.h>

/*
//Size of vector table, note yours is probably 16 entries
#define VECTOR_TABLE_ENTRIES 8
//Base address in MCU memory of vector table (by default 0x0 for ARM9)
#define HARDWARE_VECTOR_TABLE_ADDRESS 0x00000000

typedef void(*isr_vector)(void);

void load_vector_table();
void init_interrupt();
 */

void __attribute__((interrupt)) __cs3_reset();
void __attribute__((interrupt)) __cs3_isr_undef();
void __attribute__((interrupt)) __cs3_isr_swi();
void __attribute__((interrupt)) __cs3_isr_pabort();
void __attribute__((interrupt)) __cs3_isr_dabort();
void __attribute__((interrupt)) __cs3_isr_irq();
void __attribute__((interrupt)) __cs3_isr_fiq();

void config_stack_pointers();
void config_arm_interrupt();
void config_key_interrupt();
void config_priv_timer_interrupt();
void config_ps2_interrupt();

void on_key_press();
void on_priv_timer_tick();
void on_ps2_press();

#endif //MAIN_C_IRQ_HANDLER_H
