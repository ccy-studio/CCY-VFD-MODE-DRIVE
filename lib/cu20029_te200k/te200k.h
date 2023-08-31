/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-08-20 18:46:07
 * @LastEditTime: 2023-08-20 21:51:19
 */
#ifndef __TE200K_H
#define __TE200K_H

#include <Arduino.h>

#define u8 uint8_t

#define TE_VFD_RXD 1
#define TE_VFD_DTR 2

#define TE_VFD_WRITE(b) Serial1.write(b)
#define TE_VFD_WRITE_S(s) Serial1.print(PSTR(s))

void te200k_init(void);

/**
 * The cursor moves to the specified X, Y position. 
*/
void te200k_cursor_set(u8 x, u8 y);

/**
 * Specifies cursor display ON/OFF. Blink time for block cursor depends on time
 * set in Blink screen command.
*/
void te200k_cursor_display(u8 enable);

/**
 * Display power ON/OFF.
*/
void te200k_display_power(u8 enable);

/**
 * Reverse character display setting ON/OFF
*/
void te200k_display_reverse(u8 enable);

/**
 * Set display brightness level.
 * @param level 1 (01h) – 4 (04h)
*/
void te200k_display_brightness(u8 level);

/**
 * Display is cleared and cursor moves to home position (left end of 1st line).
*/
void te200k_display_clear(void);

/**
 * Current line is cleared and cursor moves to left end of current line.
*/
void te200k_line_clear(void);

/**
 * Screen blink control.
 * @param level Definable area: n = 0 (00h) Screen blink cancel.
                Default: n = 0
                n = 1 (01h) – 255 (FFh) Screen blink display.
*/
void te200k_display_blink(u8 level);


void te200k_test(void);

#endif
