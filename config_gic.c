#include "config_gic.h"
#include "interrupt_ids.h"

void config_gic() {
    enable_interrupt(KEYS_IRQ);
    enable_interrupt(MPCORE_PRIV_TIMER_IRQ);
    enable_interrupt(PS2_IRQ);

    volatile unsigned int * cpu_base = (unsigned int *) MPCORE_GIC_CPUIF;
    *(cpu_base + 1) = 0xFFFF;
    *cpu_base = 1;

    volatile unsigned int * dist_base = (unsigned int *) MPCORE_GIC_DIST;
    *dist_base = 1;
}

void enable_interrupt(int interrupt_id) {
    // ICDISER Config
    volatile int reg_offset = (interrupt_id / 32) * 4;
    volatile unsigned int dist_addr = MPCORE_GIC_DIST;
    volatile unsigned int icdiser = dist_addr + ICDISER;
    volatile unsigned int * addr = (unsigned int *) (icdiser + reg_offset);

    volatile unsigned int value = interrupt_id & 0x1F;
    value = 1 << value;

    volatile unsigned int reg_value = *addr;
    reg_value = reg_value | value;
    *addr = reg_value;

    // ICDIPTR Config
    reg_offset = (interrupt_id/4) * 4;
    volatile unsigned int icdiptr = dist_addr + ICDIPTR;
    volatile char * addr2 = (char *) (icdiptr + reg_offset);
    volatile unsigned int index = interrupt_id & 0x3;
    char value2 = CPU0;
    addr2 += index;
    //value = value << (4 * index);
    //value = (*addr) | value;
    *addr2 = value2;

}