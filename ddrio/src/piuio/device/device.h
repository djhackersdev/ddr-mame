/**
 * This module serves as a low level device implementation for a usb PIUIO card.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * Open a connected usb piuio device
 *
 * @return True if opening a piuio device was successful, false on failure
 */
bool piuio_device_open(void);

/**
 * Read raw data from the piuio device
 *
 * @param buffer Allocated buffer to read data from the device into
 * @param len Number of bytes to read
 * @return True if reading data to device was successful, false otherwise
 */
bool piuio_device_read(uint8_t* buffer, uint8_t len);

/**
 * Write raw data to the piuio device
 *
 * @param buffer Buffer with data to write
 * @param len Number of bytes to write
 * @return True if writing data to device was successful, false otherwise
 */
bool piuio_device_write(uint8_t* buffer, uint8_t len);

/**
 * Close the opened piuio device
 */
void piuio_device_close(void);