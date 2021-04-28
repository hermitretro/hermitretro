/* gpio_membrane.c: GPIO membrane support
   Copyright (c) 2017-2021 Alligator Descartes <https://hermitretro.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

   Author contact information:

   E-mail: alligator.descartes@hermitretro.com

*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include "bcm2835.h"

#include "gpio_membrane.h"
#include "pins.h"
#include "keymap.h"

//#define DEBUG
#undef DEBUG
#ifdef DEBUG
FILE *debugFile;
#endif

/** Debouncing during menu operation */
#define GPIO_MEMBRANE_DEBOUNCE_IN_MS 250L

uint8_t colPins[NUM_COLS] = {
  PERIPHERALS_GPIO_MEMBRANE_5_4_PIN,
  PERIPHERALS_GPIO_MEMBRANE_5_3_PIN,
  PERIPHERALS_GPIO_MEMBRANE_5_2_PIN,
  PERIPHERALS_GPIO_MEMBRANE_5_1_PIN,
  PERIPHERALS_GPIO_MEMBRANE_5_0_PIN
};

uint8_t rowPins[NUM_ROWS] = {
  PERIPHERALS_GPIO_MEMBRANE_8_0_PIN,
  PERIPHERALS_GPIO_MEMBRANE_8_1_PIN,
  PERIPHERALS_GPIO_MEMBRANE_8_2_PIN,
  PERIPHERALS_GPIO_MEMBRANE_8_3_PIN,
  PERIPHERALS_GPIO_MEMBRANE_8_4_PIN,
  PERIPHERALS_GPIO_MEMBRANE_8_5_PIN,
  PERIPHERALS_GPIO_MEMBRANE_8_6_PIN,
  PERIPHERALS_GPIO_MEMBRANE_8_7_PIN
};

// Keymap
input_key keyMap[NUM_ROWS][NUM_COLS] = {
  { INPUT_KEY_5, INPUT_KEY_4, INPUT_KEY_3, INPUT_KEY_2, INPUT_KEY_1 },
  { INPUT_KEY_t, INPUT_KEY_r, INPUT_KEY_e, INPUT_KEY_w, INPUT_KEY_q }, 
  { INPUT_KEY_g, INPUT_KEY_f, INPUT_KEY_d, INPUT_KEY_s, INPUT_KEY_a }, 
  { INPUT_KEY_6, INPUT_KEY_7, INPUT_KEY_8, INPUT_KEY_9, INPUT_KEY_0 }, 
  { INPUT_KEY_y, INPUT_KEY_u, INPUT_KEY_i, INPUT_KEY_o, INPUT_KEY_p },
  { INPUT_KEY_v, INPUT_KEY_c, INPUT_KEY_x, INPUT_KEY_z, INPUT_KEY_Shift_L }, 
  { INPUT_KEY_h, INPUT_KEY_j, INPUT_KEY_k, INPUT_KEY_l, INPUT_KEY_Return },
  { INPUT_KEY_b, INPUT_KEY_n, INPUT_KEY_m, INPUT_KEY_Control_R, INPUT_KEY_space }
};
input_key shiftedKeyMap[NUM_ROWS][NUM_COLS] = {
  { INPUT_KEY_5, INPUT_KEY_4, INPUT_KEY_3, INPUT_KEY_2, INPUT_KEY_1 },
  { INPUT_KEY_T, INPUT_KEY_R, INPUT_KEY_E, INPUT_KEY_W, INPUT_KEY_Q }, 
  { INPUT_KEY_G, INPUT_KEY_F, INPUT_KEY_D, INPUT_KEY_S, INPUT_KEY_A }, 
  { INPUT_KEY_S_6, INPUT_KEY_7, INPUT_KEY_8, INPUT_KEY_9, INPUT_KEY_BackSpace }, 
  { INPUT_KEY_Y, INPUT_KEY_U, INPUT_KEY_I, INPUT_KEY_O, INPUT_KEY_P },
  { INPUT_KEY_V, INPUT_KEY_C, INPUT_KEY_X, INPUT_KEY_Z, INPUT_KEY_Shift_L }, 
  { INPUT_KEY_H, INPUT_KEY_J, INPUT_KEY_K, INPUT_KEY_L, INPUT_KEY_Return },
  { INPUT_KEY_B, INPUT_KEY_N, INPUT_KEY_M, INPUT_KEY_Control_R, INPUT_KEY_space }
};
input_key symbolShiftedKeyMap[NUM_ROWS][NUM_COLS] = {
  { INPUT_KEY_SS_5, INPUT_KEY_SS_4, INPUT_KEY_SS_3, INPUT_KEY_SS_2, INPUT_KEY_SS_1 },
  { INPUT_KEY_SS_t, INPUT_KEY_SS_r, INPUT_KEY_SS_e, INPUT_KEY_SS_w, INPUT_KEY_SS_q }, 
  { INPUT_KEY_SS_g, INPUT_KEY_SS_f, INPUT_KEY_SS_d, INPUT_KEY_SS_s, INPUT_KEY_SS_a }, 
  { INPUT_KEY_SS_6, INPUT_KEY_SS_7, INPUT_KEY_SS_8, INPUT_KEY_SS_9, INPUT_KEY_SS_0 }, 
  { INPUT_KEY_SS_y, INPUT_KEY_SS_u, INPUT_KEY_SS_i, INPUT_KEY_SS_o, INPUT_KEY_SS_p },
  { INPUT_KEY_SS_v, INPUT_KEY_SS_c, INPUT_KEY_SS_x, INPUT_KEY_SS_z, INPUT_KEY_Shift_L }, 
  { INPUT_KEY_SS_h, INPUT_KEY_SS_j, INPUT_KEY_SS_k, INPUT_KEY_SS_l, INPUT_KEY_Return },
  { INPUT_KEY_SS_b, INPUT_KEY_SS_n, INPUT_KEY_SS_m, INPUT_KEY_Control_R, INPUT_KEY_space }
};

/** Pressed keys matrix to speed up unpresses... */
unsigned char keysPressed[NUM_ROWS][NUM_COLS];

extern unsigned long long lastEventTime;
extern void updateLastTime( unsigned long long *lastTime );
extern int debounceEvent( unsigned long long debounceInterval, 
                          unsigned long long lastTime );

extern int gpioInit;

int shiftPressed = 0;
int symbolShiftPressed = 0;

/** Key handling */
int pressKey( int r, int c ) {
#ifdef DEBUG
  fprintf( debugFile, "PRESS KEY: %d, %d\n", r, c );
  fflush( debugFile );
#endif

  input_key c1;
  if ( shiftPressed ) {
    c1 = shiftedKeyMap[r][c];
  } else {
    if ( symbolShiftPressed ) {
        c1 = symbolShiftedKeyMap[r][c];
    } else {
        c1 = keyMap[r][c];
    }
  }

  keysPressed[r][c] = 1;

  /**
   * Update the last event time before issuing the
   * event to avoid ghost keypresses
   */
  updateLastTime( &lastEventTime );

  return c1;
}

int unpressKey( int r, int c ) {

#ifdef DEBUG2
  fprintf( debugFile, "UNPRESS KEY: %d, %d\n", r, c );
  fflush( debugFile );
#endif

  input_key c1 = keyMap[r][c];

  keysPressed[r][c] = 0;

  return -1;

//  input_event_t fuse_event;
//  fuse_event.type = INPUT_EVENT_KEYRELEASE;
//  fuse_event.types.key.native_key = c1;
//  fuse_event.types.key.spectrum_key = c1;
//  input_event( &fuse_event );
}

/**
 * Initialise the GPIO pins to handle the membrane
 */
int
gpio_membrane_init( void )
{
#ifdef DEBUG
    debugFile = fopen( "/tmp/zxzerodiag.txt", "wb" );
#endif
    bcm2835_gpio_fsel( PERIPHERALS_GPIO_MEMBRANE_5_0_PIN, BCM2835_GPIO_FSEL_INPT );
    bcm2835_gpio_fsel( PERIPHERALS_GPIO_MEMBRANE_5_1_PIN, BCM2835_GPIO_FSEL_INPT );
    bcm2835_gpio_fsel( PERIPHERALS_GPIO_MEMBRANE_5_2_PIN, BCM2835_GPIO_FSEL_INPT );
    bcm2835_gpio_fsel( PERIPHERALS_GPIO_MEMBRANE_5_3_PIN, BCM2835_GPIO_FSEL_INPT );
    bcm2835_gpio_fsel( PERIPHERALS_GPIO_MEMBRANE_5_4_PIN, BCM2835_GPIO_FSEL_INPT );

    bcm2835_gpio_fsel( PERIPHERALS_GPIO_MEMBRANE_8_0_PIN, BCM2835_GPIO_FSEL_INPT );
    bcm2835_gpio_fsel( PERIPHERALS_GPIO_MEMBRANE_8_1_PIN, BCM2835_GPIO_FSEL_INPT );
    bcm2835_gpio_fsel( PERIPHERALS_GPIO_MEMBRANE_8_2_PIN, BCM2835_GPIO_FSEL_INPT );
    bcm2835_gpio_fsel( PERIPHERALS_GPIO_MEMBRANE_8_3_PIN, BCM2835_GPIO_FSEL_INPT );
    bcm2835_gpio_fsel( PERIPHERALS_GPIO_MEMBRANE_8_4_PIN, BCM2835_GPIO_FSEL_INPT );
    bcm2835_gpio_fsel( PERIPHERALS_GPIO_MEMBRANE_8_5_PIN, BCM2835_GPIO_FSEL_INPT );
    bcm2835_gpio_fsel( PERIPHERALS_GPIO_MEMBRANE_8_6_PIN, BCM2835_GPIO_FSEL_INPT );
    bcm2835_gpio_fsel( PERIPHERALS_GPIO_MEMBRANE_8_7_PIN, BCM2835_GPIO_FSEL_INPT );

  // set all pins as inputs and activate pullups
  for (uint8_t c = 0; c < NUM_COLS; c++)
  {
    bcm2835_gpio_fsel(colPins[c], BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set(colPins[c] );

    /** Unpress all keys */
    for (uint8_t r = 0; r < NUM_ROWS; r++)
    {
      keysPressed[r][c] = 0;
    }
  }

  return 0;
}

/**
 * Read the GPIO pins and issue appropriate press/unpress commands
 */
int
gpio_membrane_poll( void )
{

  if ( gpioInit == 0 ) {
    return -1;
  }

  if ( debounceEvent( GPIO_MEMBRANE_DEBOUNCE_IN_MS, lastEventTime ) ) {
#ifdef DEBUG2
    fprintf( debugFile, "gpio_membrane: debounce0\n" );
    fflush( debugFile );
#endif
    return -1;
  }

  /** Check for shift */
  bcm2835_gpio_fsel( rowPins[SHIFT_ROW], BCM2835_GPIO_FSEL_OUTP );
  if ( bcm2835_gpio_lev( colPins[SHIFT_COL] ) == LOW ) {
#ifdef DEBUG
    fprintf( debugFile, "SHIFT PRESSED: %d\n", lastEventTime );
    fflush( debugFile );
#endif
    shiftPressed = 1;
    updateLastTime( &lastEventTime );
  } else {
    shiftPressed = 0;
  }
  bcm2835_gpio_fsel( rowPins[SHIFT_ROW], BCM2835_GPIO_FSEL_INPT );

  /** Check for symbol shift */
  bcm2835_gpio_fsel( rowPins[SYMBOL_SHIFT_ROW], BCM2835_GPIO_FSEL_OUTP );
  if ( bcm2835_gpio_lev( colPins[SYMBOL_SHIFT_COL] ) == LOW ) {
#ifdef DEBUG
    fprintf( debugFile, "SYMBOL SHIFT PRESSED\n" );
#endif
    symbolShiftPressed = 1;
    updateLastTime( &lastEventTime );
  } else {
    symbolShiftPressed = 0;
  }
  bcm2835_gpio_fsel( rowPins[SYMBOL_SHIFT_ROW], BCM2835_GPIO_FSEL_INPT );

#ifdef DEBUG2
  fprintf( debugFile, ">>>\n" );
#endif
  /**
   * Scan the keyboard matrix by enabling each row one-by-one and testing each column
   */
  for ( uint8_t r = 0 ; r < NUM_ROWS ; r++ ) {
    bcm2835_gpio_fsel( rowPins[r], BCM2835_GPIO_FSEL_OUTP );
    bcm2835_gpio_clr( rowPins[r] );

    for ( uint8_t c = 0 ; c < NUM_COLS ; c++ ) {
#ifdef DEBUG2
      fprintf( debugFile, "[%d]", bcm2835_gpio_lev(colPins[c]) );
#endif

      /** We've already processed CAPS SHIFT and SYMBOL SHIFT so skip them here */
      if ( (r == SHIFT_ROW && c == SHIFT_COL) || (r == SYMBOL_SHIFT_ROW && c == SYMBOL_SHIFT_COL) ) {
        continue;
      }

      if ( bcm2835_gpio_lev( colPins[c] ) == LOW ) {
#ifdef DEBUG2
        fprintf( debugFile, "%d %d\n", r, c );
	fflush( debugFile );
#endif
        bcm2835_gpio_fsel( rowPins[r], BCM2835_GPIO_FSEL_INPT );
	    return pressKey( r, c );
      }
      keysPressed[r][c] = 0;
    }

    // turn the row back off
    bcm2835_gpio_fsel( rowPins[r], BCM2835_GPIO_FSEL_INPT );

#ifdef DEBUG2
    fprintf( debugFile, "\n" );
#endif
  }

#ifdef DEBUG2
    fprintf( debugFile, "<<<\n" );
    fflush( debugFile );
#endif

  bcm2835_gpio_clr( rowPins[SHIFT_ROW] );

  return -1;
}

