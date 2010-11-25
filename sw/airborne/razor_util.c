/*
 * $Id: razor_imu.h $
 *  
 * Copyright (C) 2010 Christoph Niemann
 *
 * This file is part of paparazzi.
 *
 * paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with paparazzi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA. 
 *
 */

/** \file razor_util.c
 *  \brief Razor IMU Utilities
 *
 */

#include "adc.h"
#include "uart.h"
 
#include "std.h"
#include "nav.h"
#include "estimator.h"
#include "autopilot.h"
#include "flight_plan.h"


#include "bomb.h"
#include "airframe.h"
#include "inter_mcu.h"

#include "razor_util.h" 

 
uint16_t razor_raw[NB_ADC];

// variables
/** gyro rate in rad */
volatile float gyro[G_LAST] = {0.};
/** gyro rate offset in rad */
volatile float gyro_to_zero[G_LAST] = {0.};
/** acceleration in ms2 */
volatile float accel[ACC_LAST] = {0.};
/** angle in rad */
volatile float angle[ANG_LAST] = {0.};
/** magnet heading \todo heading ? */
volatile float heading;
/** kalman period, possible self tuned */
volatile int kalman_period = 0;


// static void razor_util_init( void ) {
// 
//   
// }

void razor_delay( void ) {
  volatile int i,j;
  for (i=0;i<1000;i++)
    for (j=0;j<1000;j++);
}

bool_t  razor_imu_reset( void ) {
 #ifndef RAZOR_ZERO_AVERAGE
  gyro_to_zero[GO_ROLL] = gyro[G_ROLL];
  gyro_to_zero[GO_PITCH] = gyro[G_PITCH];
  gyro_to_zero[GO_YAW] = gyro[G_YAW];
 #else
  /** temp_value for calculating the average */
  volatile float gyro_to_zero_temp[G_LAST] = {0.};
  
  gyro_to_zero_temp[GO_ROLL] = gyro[G_ROLL];
  gyro_to_zero_temp[GO_PITCH] = gyro[G_PITCH];
  gyro_to_zero_temp[GO_YAW] = gyro[G_YAW];
  
  razor_delay();
  
  gyro_to_zero_temp[GO_ROLL] += gyro[G_ROLL];
  gyro_to_zero_temp[GO_PITCH] += gyro[G_PITCH];
  gyro_to_zero_temp[GO_YAW] += gyro[G_YAW];
  
  razor_delay();
  
  gyro_to_zero_temp[GO_ROLL] += gyro[G_ROLL];
  gyro_to_zero_temp[GO_PITCH] += gyro[G_PITCH];
  gyro_to_zero_temp[GO_YAW] += gyro[G_YAW];
  
  gyro_to_zero_temp[GO_ROLL] /= 3;
  gyro_to_zero_temp[GO_PITCH] /= 3;
  gyro_to_zero_temp[GO_YAW] /= 3;
  
  gyro_to_zero[GO_ROLL] = gyro_to_zero_temp[G_ROLL];
  gyro_to_zero[GO_PITCH] = gyro_to_zero_temp[G_PITCH];
  gyro_to_zero[GO_YAW] = gyro_to_zero_temp[G_YAW];
 #endif
  return FALSE;
}


