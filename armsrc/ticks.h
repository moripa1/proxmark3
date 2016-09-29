//-----------------------------------------------------------------------------
// Jonathan Westhues, Aug 2005
// Iceman, Sept 2016
//
// This code is licensed to you under the terms of the GNU GPL, version 2 or,
// at your option, any later version. See the LICENSE.txt file for the text of
// the license.
//-----------------------------------------------------------------------------
// Timers, Clocks functions used in LF or Legic where you would need detailed time.
//-----------------------------------------------------------------------------

#ifndef __TICKS_H
#define __TICKS_H

#include <stddef.h>
#include <stdint.h>
#include "common.h"
#include "apps.h"
#include "proxmark3.h"

#ifndef GET_TICKS
# define GET_TICKS   (uint32_t)((AT91C_BASE_TC1->TC_CV << 16) | AT91C_BASE_TC0->TC_CV)
#endif

void SpinDelay(int ms);
void SpinDelayUs(int us);

void StartTickCount(void);
uint32_t RAMFUNC GetTickCount(void);

void StartCountUS(void);
uint32_t RAMFUNC GetCountUS(void);
void ResetUSClock(void);
void SpinDelayCountUs(uint32_t us);
//uint32_t RAMFUNC GetDeltaCountUS(void);

void StartCountSspClk();
void ResetSspClk(void);
uint32_t RAMFUNC GetCountSspClk();

extern void StartTicks(void);
extern void WaitTicks(uint32_t ticks);
extern void WaitUS(uint16_t us);
extern void WaitMS(uint16_t ms);
extern void ResetTicks();
extern void ResetTimer(AT91PS_TC timer);
#endif