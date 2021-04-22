/**
 * zxzerodiag.c: Low-level diagnostics for Hermit Retro Products ZXZero board
 *
 * This file is part of Hermit Retro ZXZero
 *
 *     zxzerdiag is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     zxzerodiag is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with zxzerodiag.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <curses.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include <getopt.h>

#include "bcm2835.h"
#include "pins.h"

#define ZXZERODIAG_VERSION "1.0.2"

#define DEBOUNCE_INTERVAL 250L
unsigned long long lastEventTime = 0;

#define DEFAULT_MILLISECONDS_TO_WAIT 3000L
unsigned long long lastWaitTime = 0;

/** Terminal dimensions */
int maxlines, maxcols;

/** Use GPIO as input. If not, use keyboard */
int useGPIO = 0;

/** Compute the time between the event and now */
int secondsToWait( unsigned long long lastTime, unsigned long long waitTime ) {
    struct timeval tv;
    gettimeofday( &tv, NULL );

    unsigned long long tvnow =
        (unsigned long long)(tv.tv_sec) * 1000 +
        (unsigned long long)(tv.tv_usec) / 1000;
    return ((((lastTime + (DEFAULT_MILLISECONDS_TO_WAIT * 1000)) - tvnow) / 1000) % 10) - 6;
}

/**
 * Update the timestamp of the last valid event. Debounced events should be
 * discarded by the caller
 */
void updateLastTime( unsigned long long *lastTime ) {
    struct timeval tv;
    gettimeofday( &tv, NULL );
    *lastTime =
        (unsigned long long)(tv.tv_sec) * 1000 +
        (unsigned long long)(tv.tv_usec) / 1000;
}

/**
 * Debounce the input event against the required interval.
 * Returns: 1 if the caller should discard the duplicate event
 *          0 if the caller should use the event
 */
int debounceEvent( unsigned long long debounceInterval, unsigned long long lastTime ) {

    struct timeval tv;
    gettimeofday( &tv, NULL );
    unsigned long long t =
        (unsigned long long)(tv.tv_sec) * 1000 +
        (unsigned long long)(tv.tv_usec) / 1000;
    if ( (t - lastTime) < debounceInterval ) {
        return 1;
    }

    return 0;
}

/**
 * Blocking wait for input
 */
void blockWait() {
    if ( useGPIO ) {
        updateLastTime( &lastEventTime );

        while ( 1 ) {
            if ( debounceEvent( DEBOUNCE_INTERVAL, lastEventTime ) ) {
                continue;
            }

            if ( bcm2835_gpio_lev( HERMITRETRO_ZXZERO_FUSE_MENU_PIN ) == LOW ) {
                return;
            }
        }
    } else {
        timeout( -1 );
        getch();
    }
}

/**
 * Execute a command and return the output
 */
void execCommand( const char *command ) {
    int x = 0,
        y = 2;
    char buf[512];

    mvaddstr( 1, 0, ">>>>>>>>>>>>>>>>" );
    refresh();
    FILE *f = popen( command, "r" );
    if ( f != NULL ) {
        mvaddstr( y++, 0, "Opened pipe ok\n" );
        refresh();
        while ( fgets( buf, sizeof( buf ), f ) != NULL ) {
            addstr( buf );
        }
        pclose( f );
        addstr( "<<<<<<<<<<<<<<<<" );
    } else {
        mvaddstr( y++, 0, "Failed to open pipe" );
        mvaddstr( ++y, 0, "<<<<<<<<<<<<<<<<" );
    }
    refresh();
}

/**
 * Test GPIO pins
 */
void testGPIO() {

#define NGPIOPINS 20
    int gpioPins[NGPIOPINS] = {
        HERMITRETRO_ZXZERO_ZELUX_PWR_PIN,
        PERIPHERALS_GPIO_MEMBRANE_5_0_PIN,
        PERIPHERALS_GPIO_MEMBRANE_5_1_PIN,
        PERIPHERALS_GPIO_MEMBRANE_5_2_PIN,
        PERIPHERALS_GPIO_MEMBRANE_5_3_PIN,
        PERIPHERALS_GPIO_MEMBRANE_5_4_PIN,
        PERIPHERALS_GPIO_MEMBRANE_8_0_PIN,
        PERIPHERALS_GPIO_MEMBRANE_8_1_PIN,
        PERIPHERALS_GPIO_MEMBRANE_8_2_PIN,
        PERIPHERALS_GPIO_MEMBRANE_8_3_PIN,
        PERIPHERALS_GPIO_MEMBRANE_8_4_PIN,
        PERIPHERALS_GPIO_MEMBRANE_8_5_PIN,
        PERIPHERALS_GPIO_MEMBRANE_8_6_PIN,
        PERIPHERALS_GPIO_MEMBRANE_8_7_PIN,
        PERIPHERALS_GPIO_JOYSTICK_UP_PIN,
        PERIPHERALS_GPIO_JOYSTICK_DOWN_PIN,
        PERIPHERALS_GPIO_JOYSTICK_LEFT_PIN,
        PERIPHERALS_GPIO_JOYSTICK_RIGHT_PIN,
        PERIPHERALS_GPIO_JOYSTICK_FIRE1_PIN,
        PERIPHERALS_GPIO_JOYSTICK_FIRE2_PIN
    };

    int i;
    int x = 0, y = 2;
    int ystart = 3;
    char str[32];

    clear();

    mvaddstr( 1, 0, ">> GPIO Test\n" );
    mvaddstr( 3, 0, ">> Attach the LED Test Hat!" );
    mvaddstr( 5, 0, ">> Press a key to continue" );

    refresh();

    blockWait();

    mvaddstr( 7, 0, ">> All LEDs should be illuminated for 5 seconds then off" );
    refresh();

    for ( i = 0 ; i < NGPIOPINS ; i++ ) {
        bcm2835_gpio_fsel( gpioPins[i], BCM2835_GPIO_FSEL_OUTP );
        bcm2835_gpio_set( gpioPins[i] );
    }

    sleep( 5 );

    for ( i = 0 ; i < NGPIOPINS ; i++ ) {
        bcm2835_gpio_clr( gpioPins[i] );
    }

    mvaddstr( maxlines - 5, 0, ">>> GPIO test complete" );
    mvaddstr( maxlines - 4, 0, "Press a key to continue" );
    refresh();
    blockWait();
}

int main( int argc, char **argv ) {

    int menuIndex = 0;
    int starty = 4;

    int stw = 0, laststw = 0;

    int needsRefresh = 1;
    int neededRefresh = 0;

    char str[128];

    /** Parse args */
    int c;
    while ( (c = getopt (argc, argv, "hg")) != -1 ) {
        switch ( c ) {
            default:
            case 'h': {
                printf( "zxzerodiag: Hermit Retro Products ZXZero Diagnostics (v%s)\nUsage: zxzerodiag [-h] [-g]\n\n    -h   Display help\n    -g    Use GPIO input\n", ZXZERODIAG_VERSION );
                exit( 0 );
                break;
            }
            case 'g': {
                useGPIO = 1;
                break;
            }
        }
    }

    /* Initialize curses */
    initscr();
    cbreak();
    noecho();

    clear();

    maxlines = LINES - 1;
    maxcols = COLS - 1;

    /** Initialise BCM2835 */
    if ( !bcm2835_init() ) {
        endwin();
        printf( "failed to initialise BCM2835\n" );
        exit( 1 );
    }

    /** Initialise Fuse button pin */
    bcm2835_gpio_fsel( HERMITRETRO_ZXZERO_FUSE_MENU_PIN, BCM2835_GPIO_FSEL_INPT );

    while ( 1 ) {

        if ( debounceEvent( DEBOUNCE_INTERVAL, lastEventTime ) ) {
            continue;
        }

        /** Main processing */

        /** See if the countdown has updated */
        laststw = stw;
        stw = secondsToWait( lastWaitTime, DEFAULT_MILLISECONDS_TO_WAIT );
        if ( laststw != stw ) {
            needsRefresh = 1;
        }

        if ( needsRefresh ) {
            clear();

            /** Draw header */
            sprintf( str, "ZXZero Diagnostics (v%s) - (c)2021 Hermit Retro Products", ZXZERODIAG_VERSION );
            mvaddstr( 1, 0, str );
            mvaddstr( 2, 0, "https://hermitretro.com" );

            /** Draw the menu */
            int y = starty;
            mvaddstr( y++, 2, "0. No-operation" );
            mvaddstr( y++, 2, "1. Mount SD Card" );
            mvaddstr( y++, 2, "2. Unmount SD Card" );
            mvaddstr( y++, 2, "3. ls -laF SD Card" );
            mvaddstr( y++, 2, "4. Stress Test SD Card" );
            mvaddstr( y++, 2, "5. Test GPIO" );
            mvaddstr( y++, 2, "6. Test USB" );
            mvaddstr( y++, 2, "7. Test I2C" );
            mvaddstr( y++, 2, "8. Delete Fuse config file" );

            mvaddch( menuIndex + starty, 0, '>' );

            /** Draw footer */
            sprintf( str, "Menu selection will be activated in %lu seconds", stw );

            mvaddstr( maxlines - 2, 0, "Press Fuse menu button to advance cursor" );
            mvaddstr( maxlines - 1, 0, str );
        }

        neededRefresh = needsRefresh;
        needsRefresh = 0;

        /** Have we pressed the fuse button? */
        int buttonPressed = 0;
        if ( useGPIO ) {
            buttonPressed = (bcm2835_gpio_lev( HERMITRETRO_ZXZERO_FUSE_MENU_PIN ) == LOW);
        } else {
            timeout( 10 );
            buttonPressed = (getch() != ERR);
        }
        if ( buttonPressed ) {
            /** Update the last wait time */
            updateLastTime( &lastWaitTime );

            /** Advance the cursor */
            menuIndex++;
            menuIndex %= 9;

            needsRefresh = 1;
        } else {
            /** Potentially time out the menu option */
            if ( debounceEvent( DEFAULT_MILLISECONDS_TO_WAIT, lastWaitTime ) == 0 ) {
                /** Menu selection has timed out */
                switch ( menuIndex ) {
                    default:
                    case 0: {
                        /** Do nothing */
                        mvaddstr( maxlines - 5, 0, ">>> NO-OP" );
                        mvaddstr( maxlines - 4, 0, "Press a key to continue" );
                        refresh();
                        break;
                    }
                    case 1: {
                        mvaddstr( maxlines - 5, 0, ">>> Mount SD Card" );
                        mvaddstr( maxlines - 4, 0, "Press a key to continue" );
                        refresh();
                        blockWait();
                        clear();
                        move( 0, 0 );
                        mvaddstr( 0, 0, ">>> Mount SD Card" );
                        execCommand( "/usr/bin/spi-fat-fuse /mnt/sd 2>&1" );
                        mvaddstr( maxlines - 4, 0, "Press a key to continue" );
                        blockWait();
                        break;
                    }
                    case 2: {
                        mvaddstr( maxlines - 5, 0, ">>> Unmount SD Card" );
                        mvaddstr( maxlines - 4, 0, "Press a key to continue" );
                        refresh();
                        blockWait();
                        clear();
                        move( 0, 0 );
                        mvaddstr( 0, 0, ">>> Unmount SD Card" );
                        execCommand( "fusermount3 -u /mnt/sd 2>&1" );
                        mvaddstr( maxlines - 4, 0, "Press a key to continue" );
                        blockWait();
                        break;
                    }
                    case 3: {
                        mvaddstr( maxlines - 5, 0, ">>> ls -laF SD Card" );
                        mvaddstr( maxlines - 4, 0, "Press a key to continue" );
                        refresh();
                        blockWait();
                        clear();
                        refresh();
                        move( 0, 0 );
                        mvaddstr( 0, 0, ">>> ls -laF SD Card" );
                        refresh();
                        execCommand( "/bin/ls -laF /mnt/sd 2>&1" );
                        mvaddstr( maxlines - 4, 0, "Press a key to continue" );
                        refresh();
                        blockWait();
                        break;
                    }
                    case 4: {
                        mvaddstr( maxlines - 5, 0, ">>> Stress SD Card" );
                        mvaddstr( maxlines - 4, 0, "Press a key to continue" );
                        refresh();
                        blockWait();
                        clear();
                        move( 0, 0 );
                        mvaddstr( 0, 0, ">>> Stress SD Card -- this may take some time" );
                        execCommand( "/usr/bin/stresssd 2>&1" );
                        mvaddstr( maxlines - 4, 0, "Press a key to continue" );
                        blockWait();
                        break;
                    }
                    case 5: {
                        mvaddstr( maxlines - 5, 0, ">>> Test GPIO" );
                        mvaddstr( maxlines - 4, 0, "Press a key to continue" );
                        refresh();
                        blockWait();
                        testGPIO();
                        blockWait();
                        break;
                    }
                    case 6: {
                        mvaddstr( maxlines - 5, 0, ">>> Test USB" );
                        mvaddstr( maxlines - 4, 0, "Press a key to continue" );
                        refresh();
                        blockWait();
                        clear();
                        move( 0, 0 );
                        mvaddstr( 0, 0, ">>> Test USB" );
                        execCommand( "/usr/bin/lsusb 2>&1" );
                        mvaddstr( maxlines - 4, 0, "Press a key to continue" );
                        blockWait();
                        break;
                    }
                    case 7: {
                        mvaddstr( maxlines - 5, 0, ">>> Test I2C" );
                        mvaddstr( maxlines - 4, 0, "Press a key to continue" );
                        refresh();
                        blockWait();
                        clear();
                        move( 0, 0 );
                        mvaddstr( 0, 0, ">>> Test I2C" );
                        execCommand( "/usr/sbin/i2cdetect -y 1 2>&1" );
                        mvaddstr( maxlines - 4, 0, "Press a key to continue" );
                        blockWait();
                        break;
                    }
                    case 8: {
                        mvaddstr( maxlines - 5, 0, ">>> Delete Fuse config file" );
                        mvaddstr( maxlines - 4, 0, "Press a key to continue" );
                        refresh();
                        blockWait();
                        clear();
                        move( 0, 0 );
                        mvaddstr( 0, 0, ">>> Delete Fuse config file" );
                        execCommand( "/bin/rm -f /.fuserc" );
                        execCommand( "/bin/sync" );
                        execCommand( "/bin/ls /.fuserc" );
                        mvaddstr( maxlines - 4, 0, "Press a key to continue" );
                        blockWait();
                        break;
                    }
                }

                /** Update the last wait time */
                updateLastTime( &lastWaitTime );

                /** Reset the menu to NOP */
                menuIndex = 0;
            }
        }

        /** Draw the results box */
        if ( neededRefresh ) {
            refresh();
        }

        /** Update the button/key debounce time */
        updateLastTime( &lastEventTime );

    }

    endwin();

    exit(0);
}
