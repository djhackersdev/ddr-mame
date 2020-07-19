/**
 * Common API to use for any devices implementing (MK6) piuio controls
 * (e.g. keyboard emulation, custom IOs)
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

enum piuio_itg_player {
    PIUIO_ITG_PLAYER_1 = 0,
    PIUIO_ITG_PLAYER_2 = 1,
    PIUIO_ITG_PLAYER_NUM = 2,
};

/**
 * Enum for pad input sensor groups (four sensors per button/panel)
 */
enum piuio_itg_sensor_group {
    PIUIO_ITG_SENSOR_GROUP_RIGHT = 0,
    PIUIO_ITG_SENSOR_GROUP_LEFT = 1,
    PIUIO_ITG_SENSOR_GROUP_DOWN = 2,
    PIUIO_ITG_SENSOR_GROUP_UP = 3,
    PIUIO_ITG_SENSOR_GROUP_NUM = 4,
};

struct piuio_itg_pad_input {
    bool left;
    bool down;
    bool up;
    bool right;
};

struct piuio_itg_button_input {
    bool start;
    bool back;
    bool left;
    bool right;
};

struct piuio_itg_sys_input {
    bool test;
    bool service;
    bool clear;
    bool coin;
    bool coin2;
};

struct piuio_itg_pad_output {
    bool left;
    bool down;
    bool up;
    bool right;
};

struct piuio_itg_cabinet_output {
    bool bass;
    bool halo_r2;
    bool halo_r1;
    bool halo_l2;
    bool halo_l1;
};

/**
 * Open the piuio device
 *
 * Open any file handles, spawn threads and/or initialize your device or emulation code to be ready to accept further
 * commands.
 *
 * @return True if opening/initializing your implementation was sucessful, false on failure
 */
bool piuio_itg_open(void);

/**
 * Close and cleanup your device, shut down threads, free resources etc
 */
void piuio_itg_close(void);

/**
 * Get _ALL_ readable data from your device in this single recv call. If you have to address single sensores like the
 * real piuio, you have to do this on a single call to this function.
 *
 * @return True on success, false on error
 */
bool piuio_itg_recv(void);

/**
 * Send any output data (e.g. lights) to your device
 *
 * @return True on success, false on error
 */
bool piuio_itg_send(void);

/**
 * Get buffered pad input data
 *
 * @param player Player id (0, 1)
 * @param sensor_group Sensor group states to get
 * @param inputs Pointer to buffer to copy inputs of the selected sensor group to. Every time this is invoked, this
 *               buffer is nulled. If you do not write any data to this, all inputs remain un-triggered.
 */
void piuio_itg_get_input_pad(enum piuio_itg_player player, enum piuio_itg_sensor_group sensor_group,
        struct piuio_itg_pad_input* input);

void piuio_itg_get_input_button(enum piuio_itg_player player, struct piuio_itg_button_input* input);

/**
 * Get buffered system (test, service, clear, coin) inputs
 *
 * @param inputs Pointer to buffer to copy inputs to. Every time this is invoked, this buffer is nulled. If you do not
 *               write any data to this, all inputs remain un-triggered.
 */
void piuio_itg_get_input_sys(struct piuio_itg_sys_input* inputs);

/**
 * Set pad light outputs (i.e. buffer them)
 *
 * @param player Player id (0, 1)
 * @param outputs Pointer to output data to copy to your private buffers.
 */
void piuio_itg_set_output_pad(enum piuio_itg_player player, const struct piuio_itg_pad_output* output);

/**
 * Set cabinet light outputs (i.e. buffer them)
 *
 * @param outputs Pointer to output data to copy to your private buffers.
 */
void piuio_itg_set_output_cabinet(const struct piuio_itg_cabinet_output* output);