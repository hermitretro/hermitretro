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

/** Pinched from the fuse-hermitretro peripherals */
#define HERMITRETRO_ZXZERO_FUSE_MENU_PIN RPI_BPLUS_GPIO_J8_15
#define HERMITRETRO_ZXZERO_ZELUX_PWR_PIN 0  /** ID_SD, physical pin 27 */

#define PERIPHERALS_GPIO_MEMBRANE_5_0_PIN RPI_BPLUS_GPIO_J8_37
#define PERIPHERALS_GPIO_MEMBRANE_5_1_PIN RPI_BPLUS_GPIO_J8_35
#define PERIPHERALS_GPIO_MEMBRANE_5_2_PIN RPI_BPLUS_GPIO_J8_40
#define PERIPHERALS_GPIO_MEMBRANE_5_3_PIN RPI_BPLUS_GPIO_J8_38
#define PERIPHERALS_GPIO_MEMBRANE_5_4_PIN RPI_BPLUS_GPIO_J8_36

#define PERIPHERALS_GPIO_MEMBRANE_8_0_PIN RPI_BPLUS_GPIO_J8_33
#define PERIPHERALS_GPIO_MEMBRANE_8_1_PIN RPI_BPLUS_GPIO_J8_31
#define PERIPHERALS_GPIO_MEMBRANE_8_2_PIN RPI_BPLUS_GPIO_J8_29
#define PERIPHERALS_GPIO_MEMBRANE_8_3_PIN RPI_BPLUS_GPIO_J8_32
#define PERIPHERALS_GPIO_MEMBRANE_8_4_PIN RPI_BPLUS_GPIO_J8_26
#define PERIPHERALS_GPIO_MEMBRANE_8_5_PIN RPI_BPLUS_GPIO_J8_22
#define PERIPHERALS_GPIO_MEMBRANE_8_6_PIN RPI_BPLUS_GPIO_J8_18
#define PERIPHERALS_GPIO_MEMBRANE_8_7_PIN RPI_BPLUS_GPIO_J8_16

#define PERIPHERALS_GPIO_JOYSTICK_UP_PIN RPI_BPLUS_GPIO_J8_07
#define PERIPHERALS_GPIO_JOYSTICK_DOWN_PIN RPI_BPLUS_GPIO_J8_08
#define PERIPHERALS_GPIO_JOYSTICK_LEFT_PIN RPI_BPLUS_GPIO_J8_10
#define PERIPHERALS_GPIO_JOYSTICK_RIGHT_PIN RPI_BPLUS_GPIO_J8_12
#define PERIPHERALS_GPIO_JOYSTICK_FIRE1_PIN RPI_BPLUS_GPIO_J8_11
#define PERIPHERALS_GPIO_JOYSTICK_FIRE2_PIN RPI_BPLUS_GPIO_J8_13

