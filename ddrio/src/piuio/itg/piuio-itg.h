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

bool piuio_itg_open(void);

void piuio_itg_close(void);

bool piuio_itg_recv(void);

bool piuio_itg_send(void);

void piuio_itg_get_input_pad(enum piuio_itg_player player, struct piuio_itg_pad_input* input);

void piuio_itg_get_input_button(enum piuio_itg_player player, struct piuio_itg_button_input* input);

void piuio_itg_get_input_sys(struct piuio_itg_sys_input* inputs);

void piuio_itg_set_output_pad_sensor_group(enum piuio_itg_player player, enum piuio_itg_sensor_group sensor_group);

void piuio_itg_set_output_pad(enum piuio_itg_player player, const struct piuio_itg_pad_output* output);

void piuio_itg_set_output_cabinet(const struct piuio_itg_cabinet_output* output);