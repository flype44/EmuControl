#ifndef _MBOX_H
#define _MBOX_H

#include <exec/types.h>
#include <stdint.h>

ULONG get_core_temperature();
ULONG get_core_voltage();
ULONG get_vc_memory_size();
ULONG get_firmware_revision();
ULONG get_clock_rate(ULONG id);
ULONG get_clock_rate_measured(ULONG id);
ULONG get_clock_rate_max(ULONG id);
ULONG get_board_revision();
ULONG get_turbo_mode();
void get_board_serial(ULONG *hi, ULONG *lo);
void get_board_macaddr(ULONG *hi, ULONG *lo);
void get_framebuffer_size(ULONG *width, ULONG *height);
void reboot_rpi_firmware(BOOL kill_exec);

#endif /* _MBOX_H */
