/*
 * Copyright (c) 2009 by Stefan Siegl <stesie@brokenpipe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * For more information on the GPL, please go to:
 * http://www.gnu.org/copyleft/gpl.html
 */

/*
 *
 * Parts of this file are derived from Open HR20 LCD Driver, which is
 * distributed unter GPLv2+ and
 *
 * Copyright (C) 2008 Dario Carluccio (hr20-at-carluccio-dot-de)
 *		 2009 Thomas Vosshagen (mod. for THERMOTronic) (openhr20-at-vosshagen-dot-com)
 */

#include <stdlib.h>
#include <string.h>

#include "config.h"

#include "protocols/ecmd/ecmd-base.h"

#define HR20_LCD_INITIAL_CONTRAST   14

#define LCD_SEG_SET(i)		((&LCDDR0)[(i)/8] |=  (1 << ((i) & 7)))
#define LCD_SEG_CLEAR(i)	((&LCDDR0)[(i)/8] &= ~(1 << ((i) & 7)))
#define LCD_SEG_TOGGLE(i)	((&LCDDR0)[(i)/8] ^=  (1 << ((i) & 7)))

void
hr20_lcd_init (void)
{
  /* Clear display (i.e. write zero to LCDDR0..LCDDR19). */
  memset ((void *) &LCDDR0, 0, 20);

  /* Set the initial LCD contrast level */
  LCDCCR = HR20_LCD_INITIAL_CONTRAST << LCDCC0;

  /* LCD Control and Status Register B
	- clock source is TOSC1 pin
	- COM0:2 connected
	- SEG0:22 connected */
  LCDCRB = (1<<LCDCS) | (1<<LCDMUX1) | (1<<LCDPM2)| (1<<LCDPM0);

  /* LCD Frame Rate Register
	- LCD Prescaler Select N=16       @32.768Hz ->   2048Hz
	- LCD Duty Cycle 1/3 (K=6)       2048Hz / 6 -> 341,33Hz
	- LCD Clock Divider  (D=5)     341,33Hz / 7 ->  47,76Hz */
  LCDFRR = ((1<<LCDCD2)|(1<<LCDCD1));

  /* LCD Control and Status Register A
	- Enable LCD
	- Set Low Power Waveform */
  LCDCRA = (1<<LCDEN) | (1<<LCDAB);

  LCD_SEG_SET (LCD_SEG_MOON);
  LCD_SEG_SET (LCD_SEG_AUTO);
}


int16_t parse_cmd_hr20_toggle(char *cmd, char *output, uint16_t len)
{
  uint8_t i = atoi (cmd);
  if (i >= 160)
    return ECMD_ERR_PARSE_ERROR;

  LCD_SEG_TOGGLE (i);
  return ECMD_FINAL_OK;
}

/*
  -- Ethersex META --
  init(hr20_lcd_init)
  ecmd_feature(hr20_toggle, "hr20 toggle ", SEG, Toggle segment SEG (a number, not a symbolic name!))
*/