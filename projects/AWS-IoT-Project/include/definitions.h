// Author: Gustavo Diaz Galeas (Incapamentum)
//
// Last revision: April 25th, 2022

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define BAUD_RATE 9600
#define BUDGET 384
#define MAX_SIZE 512

#define EST -5

//  ===============================
//         TIME MANAGEMENT
// ===============================
#define SEC_IN_MILLIS 1000
#define SEC_IN_MICROS 1000000
#define HALF_SEC 500
#define POLL_RATE 30 * SEC_IN_MILLIS
#define T_DELAY HALF_SEC * 6
// ===============================

// ===============================
//            DHT SENSOR
// ===============================
#define DHT11_PIN 2
#define DHT_FAILURE 0
#define DHT_SUCCESS 1
// ===============================

#endif