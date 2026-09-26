/*
    Copyright © 2021 Michal Schulz <michal.schulz@gmx.de>
    https://github.com/michalsc

    This Source Code Form is subject to the terms of the
    Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
    with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/


#include <exec/types.h>
#include <exec/execbase.h>
#include <exec/io.h>
#include <exec/errors.h>

#include <proto/exec.h>
#include <proto/expansion.h>
#include <proto/devicetree.h>
#include <proto/mailbox.h>

#include <libraries/configregs.h>
#include <libraries/configvars.h>

#include <stdint.h>

/* status register flags */

#define MBOX_TX_FULL (1UL << 31)
#define MBOX_RX_EMPTY (1UL << 30)
#define MBOX_CHANMASK 0xF

extern APTR MailBox;
extern APTR MailboxBase;

UBYTE __req[256];

static inline uint32_t LE32(uint32_t x) { return __builtin_bswap32(x); }

static uint32_t mbox_recv(uint32_t channel)
{
    volatile uint32_t *mbox_read = (uint32_t*)(MailBox);
    volatile uint32_t *mbox_status = (uint32_t*)((uintptr_t)MailBox + 0x18);
    uint32_t response, status;

    do
    {
        do
        {
            status = LE32(*mbox_status);
            asm volatile("nop");
        }
        while (status & MBOX_RX_EMPTY);

        asm volatile("nop");
        response = LE32(*mbox_read);
        asm volatile("nop");
    }
    while ((response & MBOX_CHANMASK) != channel);

    return (response & ~MBOX_CHANMASK);
}

static void mbox_send(uint32_t channel, uint32_t data)
{
    volatile uint32_t *mbox_write = (uint32_t*)((uintptr_t)MailBox + 0x20);
    volatile uint32_t *mbox_status = (uint32_t*)((uintptr_t)MailBox + 0x18);
    uint32_t status;

    data &= ~MBOX_CHANMASK;
    data |= channel & MBOX_CHANMASK;

    do
    {
        status = LE32(*mbox_status);
        asm volatile("nop");
    }
    while (status & MBOX_TX_FULL);

    asm volatile("nop");
    *mbox_write = LE32(data);
}

ULONG get_core_temperature()
{
    if (MailboxBase)
    {
        ULONG *FBReq = (ULONG*)__req;

        FBReq[0] = 4*8;
        FBReq[1] = 0;
        FBReq[2] = 0x00030006;
        FBReq[3] = 8;
        FBReq[4] = 0;
        FBReq[5] = 0;
        FBReq[6] = 0;
        FBReq[7] = 0;
        
        MB_RawCommand(FBReq);

        return FBReq[6];
    }
    else
    {
        struct ExecBase *SysBase = *(struct ExecBase **)4;

        ULONG *FBReq = (ULONG*)(((ULONG)__req + 31) & ~31);
        ULONG len = 8*4;

        FBReq[0] = LE32(4*8);
        FBReq[1] = 0;
        FBReq[2] = LE32(0x00030006);
        FBReq[3] = LE32(8);
        FBReq[4] = 0;
        FBReq[5] = 0;
        FBReq[6] = 0;
        FBReq[7] = 0;

        CachePreDMA(FBReq, &len, 0);
        mbox_send(8, (ULONG)FBReq);
        mbox_recv(8);
        CachePostDMA(FBReq, &len, 0);

        return LE32(FBReq[6]);
    }
}

ULONG get_vc_memory_size()
{
    if (MailboxBase)
    {
        ULONG *FBReq = (ULONG*)__req;

        FBReq[0] = 4*8;
        FBReq[1] = 0;
        FBReq[2] = 0x00010006; /* TAG_GET_VC_MEMORY */
        FBReq[3] = 8;
        FBReq[4] = 0;
        FBReq[5] = 0;
        FBReq[6] = 0;
        FBReq[7] = 0;

        MB_RawCommand(FBReq);

        return FBReq[6];
    }
    else
    {
        struct ExecBase *SysBase = *(struct ExecBase **)4;

        ULONG *FBReq = (ULONG*)(((ULONG)__req + 31) & ~31);
        ULONG len = 8*4;

        FBReq[0] = LE32(4*8);
        FBReq[1] = 0;
        FBReq[2] = LE32(0x00010006);
        FBReq[3] = LE32(8);
        FBReq[4] = 0;
        FBReq[5] = 0;
        FBReq[6] = 0;
        FBReq[7] = 0;

        CachePreDMA(FBReq, &len, 0);
        mbox_send(8, (ULONG)FBReq);
        mbox_recv(8);
        CachePostDMA(FBReq, &len, 0);

        return LE32(FBReq[6]);
    }
}

ULONG get_firmware_revision()
{
    if (MailboxBase)
    {
        ULONG *FBReq = (ULONG*)__req;

        FBReq[0] = 4*7;
        FBReq[1] = 0;
        FBReq[2] = 0x00000001; /* TAG_GET_FIRMWARE_REV */
        FBReq[3] = 4;
        FBReq[4] = 0;
        FBReq[5] = 0;
        FBReq[6] = 0;

        MB_RawCommand(FBReq);

        return FBReq[5];
    }
    else
    {
        struct ExecBase *SysBase = *(struct ExecBase **)4;

        ULONG *FBReq = (ULONG*)(((ULONG)__req + 31) & ~31);
        ULONG len = 7*4;

        FBReq[0] = LE32(4*7);
        FBReq[1] = 0;
        FBReq[2] = LE32(0x00000001);
        FBReq[3] = LE32(4);
        FBReq[4] = 0;
        FBReq[5] = 0;
        FBReq[6] = 0;

        CachePreDMA(FBReq, &len, 0);
        mbox_send(8, (ULONG)FBReq);
        mbox_recv(8);
        CachePostDMA(FBReq, &len, 0);

        return LE32(FBReq[5]);
    }
}

ULONG get_clock_rate(ULONG id)
{
    if (MailboxBase)
    {
        ULONG *FBReq = (ULONG*)__req;

        FBReq[0] = 4*8;
        FBReq[1] = 0;
        FBReq[2] = 0x00030002; /* TAG_GET_CLOCK_RATE */
        FBReq[3] = 8;
        FBReq[4] = 0;
        FBReq[5] = id;
        FBReq[6] = 0;
        FBReq[7] = 0;

        MB_RawCommand(FBReq);

        return FBReq[6];
    }
    else
    {
        struct ExecBase *SysBase = *(struct ExecBase **)4;

        ULONG *FBReq = (ULONG*)(((ULONG)__req + 31) & ~31);
        ULONG len = 8*4;

        FBReq[0] = LE32(4*8);
        FBReq[1] = 0;
        FBReq[2] = LE32(0x00030002);
        FBReq[3] = LE32(8);
        FBReq[4] = 0;
        FBReq[5] = LE32(id);
        FBReq[6] = 0;
        FBReq[7] = 0;

        CachePreDMA(FBReq, &len, 0);
        mbox_send(8, (ULONG)FBReq);
        mbox_recv(8);
        CachePostDMA(FBReq, &len, 0);

        return LE32(FBReq[6]);
    }
}

/* Measured clock rate (as opposed to the configured rate from get_clock_rate),
   used e.g. for the framebuffer pixel clock */
ULONG get_clock_rate_measured(ULONG id)
{
    if (MailboxBase)
    {
        ULONG *FBReq = (ULONG*)__req;

        FBReq[0] = 4*8;
        FBReq[1] = 0;
        FBReq[2] = 0x00030047; /* TAG_GET_CLOCK_RATE_M */
        FBReq[3] = 8;
        FBReq[4] = 0;
        FBReq[5] = id;
        FBReq[6] = 0;
        FBReq[7] = 0;

        MB_RawCommand(FBReq);

        return FBReq[6];
    }
    else
    {
        struct ExecBase *SysBase = *(struct ExecBase **)4;

        ULONG *FBReq = (ULONG*)(((ULONG)__req + 31) & ~31);
        ULONG len = 8*4;

        FBReq[0] = LE32(4*8);
        FBReq[1] = 0;
        FBReq[2] = LE32(0x00030047);
        FBReq[3] = LE32(8);
        FBReq[4] = 0;
        FBReq[5] = LE32(id);
        FBReq[6] = 0;
        FBReq[7] = 0;

        CachePreDMA(FBReq, &len, 0);
        mbox_send(8, (ULONG)FBReq);
        mbox_recv(8);
        CachePostDMA(FBReq, &len, 0);

        return LE32(FBReq[6]);
    }
}

/* Maximum allowed clock rate: reflects config.txt arm_freq/over_voltage, so it
   already accounts for a user's overclock rather than a fixed stock value */
ULONG get_clock_rate_max(ULONG id)
{
    if (MailboxBase)
    {
        ULONG *FBReq = (ULONG*)__req;

        FBReq[0] = 4*8;
        FBReq[1] = 0;
        FBReq[2] = 0x00030004; /* TAG_GET_CLOCKRATE_MAX */
        FBReq[3] = 8;
        FBReq[4] = 0;
        FBReq[5] = id;
        FBReq[6] = 0;
        FBReq[7] = 0;

        MB_RawCommand(FBReq);

        return FBReq[6];
    }
    else
    {
        struct ExecBase *SysBase = *(struct ExecBase **)4;

        ULONG *FBReq = (ULONG*)(((ULONG)__req + 31) & ~31);
        ULONG len = 8*4;

        FBReq[0] = LE32(4*8);
        FBReq[1] = 0;
        FBReq[2] = LE32(0x00030004);
        FBReq[3] = LE32(8);
        FBReq[4] = 0;
        FBReq[5] = LE32(id);
        FBReq[6] = 0;
        FBReq[7] = 0;

        CachePreDMA(FBReq, &len, 0);
        mbox_send(8, (ULONG)FBReq);
        mbox_recv(8);
        CachePostDMA(FBReq, &len, 0);

        return LE32(FBReq[6]);
    }
}

/* Raw 32-bit board revision code: encodes board type, processor, manufacturer,
   memory size and over-voltage flag - see Emu68Info's GetBoard() bit layout */
ULONG get_board_revision()
{
    if (MailboxBase)
    {
        ULONG *FBReq = (ULONG*)__req;

        FBReq[0] = 4*7;
        FBReq[1] = 0;
        FBReq[2] = 0x00010002; /* TAG_GET_BOARD_REVISION */
        FBReq[3] = 4;
        FBReq[4] = 0;
        FBReq[5] = 0;
        FBReq[6] = 0;

        MB_RawCommand(FBReq);

        return FBReq[5];
    }
    else
    {
        struct ExecBase *SysBase = *(struct ExecBase **)4;

        ULONG *FBReq = (ULONG*)(((ULONG)__req + 31) & ~31);
        ULONG len = 7*4;

        FBReq[0] = LE32(4*7);
        FBReq[1] = 0;
        FBReq[2] = LE32(0x00010002);
        FBReq[3] = LE32(4);
        FBReq[4] = 0;
        FBReq[5] = 0;
        FBReq[6] = 0;

        CachePreDMA(FBReq, &len, 0);
        mbox_send(8, (ULONG)FBReq);
        mbox_recv(8);
        CachePostDMA(FBReq, &len, 0);

        return LE32(FBReq[5]);
    }
}

ULONG get_turbo_mode()
{
    if (MailboxBase)
    {
        ULONG *FBReq = (ULONG*)__req;

        FBReq[0] = 4*8;
        FBReq[1] = 0;
        FBReq[2] = 0x00030009; /* TAG_GET_TURBO */
        FBReq[3] = 8;
        FBReq[4] = 0;
        FBReq[5] = 0;
        FBReq[6] = 0;
        FBReq[7] = 0;

        MB_RawCommand(FBReq);

        return FBReq[6];
    }
    else
    {
        struct ExecBase *SysBase = *(struct ExecBase **)4;

        ULONG *FBReq = (ULONG*)(((ULONG)__req + 31) & ~31);
        ULONG len = 8*4;

        FBReq[0] = LE32(4*8);
        FBReq[1] = 0;
        FBReq[2] = LE32(0x00030009);
        FBReq[3] = LE32(8);
        FBReq[4] = 0;
        FBReq[5] = 0;
        FBReq[6] = 0;
        FBReq[7] = 0;

        CachePreDMA(FBReq, &len, 0);
        mbox_send(8, (ULONG)FBReq);
        mbox_recv(8);
        CachePostDMA(FBReq, &len, 0);

        return LE32(FBReq[6]);
    }
}

/* Board serial number (64 bit, as 2 words). Unlike every other property, the
   firmware hands this one back as a raw byte blob rather than two proper
   32-bit values, so it needs no byte-swap at all in either path - confirmed
   by working out the math from an actual wrong-looking reading (with a swap
   applied, "10000000c937f849" - the expected RPi4 new-style serial - came out
   as "0000001049f837c9") */
void get_board_serial(ULONG *hi, ULONG *lo)
{
    if (MailboxBase)
    {
        ULONG *FBReq = (ULONG*)__req;

        FBReq[0] = 4*8;
        FBReq[1] = 0;
        FBReq[2] = 0x00010004; /* TAG_GET_BOARD_SERIAL */
        FBReq[3] = 8;
        FBReq[4] = 0;
        FBReq[5] = 0;
        FBReq[6] = 0;
        FBReq[7] = 0;

        MB_RawCommand(FBReq);

        if (hi) *hi = FBReq[5];
        if (lo) *lo = FBReq[6];
    }
    else
    {
        /* Home-made mailbox: nothing swaps the VC4-endian buffer for us, so
           the request fields still need LE32(), just not the response here */
        struct ExecBase *SysBase = *(struct ExecBase **)4;

        ULONG *FBReq = (ULONG*)(((ULONG)__req + 31) & ~31);
        ULONG len = 8*4;

        FBReq[0] = LE32(4*8);
        FBReq[1] = 0;
        FBReq[2] = LE32(0x00010004);
        FBReq[3] = LE32(8);
        FBReq[4] = 0;
        FBReq[5] = 0;
        FBReq[6] = 0;
        FBReq[7] = 0;

        CachePreDMA(FBReq, &len, 0);
        mbox_send(8, (ULONG)FBReq);
        mbox_recv(8);
        CachePostDMA(FBReq, &len, 0);

        if (hi) *hi = FBReq[5];
        if (lo) *lo = FBReq[6];
    }
}

/* Board MAC address (48 bit, as 2 words). Unlike most other properties -
   including on MB_RawCommand, which does NOT auto-swap this particular tag,
   confirmed by testing on real mailbox.resource hardware - both words need a
   manual LE32() in both paths. get_board_serial() right above is the
   exception that doesn't need swapping at all; this one does, in full */
void get_board_macaddr(ULONG *hi, ULONG *lo)
{
    if (MailboxBase)
    {
        ULONG *FBReq = (ULONG*)__req;

        FBReq[0] = 4*8;
        FBReq[1] = 0;
        FBReq[2] = 0x00010003; /* TAG_GET_BOARD_MACADDR */
        FBReq[3] = 8;
        FBReq[4] = 0;
        FBReq[5] = 0;
        FBReq[6] = 0;
        FBReq[7] = 0;

        MB_RawCommand(FBReq);

        if (hi) *hi = LE32(FBReq[5]);
        if (lo) *lo = LE32(FBReq[6]);
    }
    else
    {
        struct ExecBase *SysBase = *(struct ExecBase **)4;

        ULONG *FBReq = (ULONG*)(((ULONG)__req + 31) & ~31);
        ULONG len = 8*4;

        FBReq[0] = LE32(4*8);
        FBReq[1] = 0;
        FBReq[2] = LE32(0x00010003);
        FBReq[3] = LE32(8);
        FBReq[4] = 0;
        FBReq[5] = 0;
        FBReq[6] = 0;
        FBReq[7] = 0;

        CachePreDMA(FBReq, &len, 0);
        mbox_send(8, (ULONG)FBReq);
        mbox_recv(8);
        CachePostDMA(FBReq, &len, 0);

        if (hi) *hi = LE32(FBReq[5]);
        if (lo) *lo = LE32(FBReq[6]);
    }
}

/* Physical (display) framebuffer size in pixels - cleaner and more reliable
   than parsing ".fbwidth="/".fbheight=" out of the devicetree bootargs
   string, which (like ".mem_size=") isn't guaranteed to be present */
void get_framebuffer_size(ULONG *width, ULONG *height)
{
    if (MailboxBase)
    {
        ULONG *FBReq = (ULONG*)__req;

        FBReq[0] = 4*8;
        FBReq[1] = 0;
        FBReq[2] = 0x00040003; /* TAG_GET_PHYSICAL_SIZE */
        FBReq[3] = 8;
        FBReq[4] = 0;
        FBReq[5] = 0;
        FBReq[6] = 0;
        FBReq[7] = 0;

        MB_RawCommand(FBReq);

        if (width) *width = FBReq[5];
        if (height) *height = FBReq[6];
    }
    else
    {
        struct ExecBase *SysBase = *(struct ExecBase **)4;

        ULONG *FBReq = (ULONG*)(((ULONG)__req + 31) & ~31);
        ULONG len = 8*4;

        FBReq[0] = LE32(4*8);
        FBReq[1] = 0;
        FBReq[2] = LE32(0x00040003);
        FBReq[3] = LE32(8);
        FBReq[4] = 0;
        FBReq[5] = 0;
        FBReq[6] = 0;
        FBReq[7] = 0;

        CachePreDMA(FBReq, &len, 0);
        mbox_send(8, (ULONG)FBReq);
        mbox_recv(8);
        CachePostDMA(FBReq, &len, 0);

        if (width) *width = LE32(FBReq[5]);
        if (height) *height = LE32(FBReq[6]);
    }
}

ULONG get_core_voltage()
{
    if (MailboxBase)
    {
        ULONG *FBReq = (ULONG*)__req;

        FBReq[0] = 4*8;
        FBReq[1] = 0;
        FBReq[2] = 0x00030003;
        FBReq[3] = 8;
        FBReq[4] = 0;
        FBReq[5] = 1;
        FBReq[6] = 0;
        FBReq[7] = 0;
        
        MB_RawCommand(FBReq);

        return FBReq[6];
    }
    else
    {
        struct ExecBase *SysBase = *(struct ExecBase **)4;

        ULONG *FBReq = (ULONG*)(((ULONG)__req + 31) & ~31);
        ULONG len = 8*4;

        FBReq[0] = LE32(4*8);
        FBReq[1] = 0;
        FBReq[2] = LE32(0x00030003);
        FBReq[3] = LE32(8);
        FBReq[4] = 0;
        FBReq[5] = LE32(1);
        FBReq[6] = 0;
        FBReq[7] = 0;

        CachePreDMA(FBReq, &len, 0);
        mbox_send(8, (ULONG)FBReq);
        mbox_recv(8);
        CachePostDMA(FBReq, &len, 0);

        return LE32(FBReq[6]);
    }
}
