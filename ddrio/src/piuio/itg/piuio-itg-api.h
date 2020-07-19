#pragma once

#include "piuio-itg.h"

typedef bool (*piuio_itg_open_t)(void);
typedef void (*piuio_itg_close_t)(void);
typedef bool (*piuio_itg_recv_t)(void);
typedef bool (*piuio_itg_send_t)(void);
typedef void (*piuio_itg_get_input_pad_t)(uint8_t player,  enum piuio_itg_sensor_group sensor_group,
    struct piuio_itg_pad_inputs* inputs);
typedef void (*piuio_itg_get_input_button_t)(uint8_t player, struct piuio_itg_button_inputs* inputs);
typedef void (*piuio_itg_get_input_sys_t)(struct piuio_itg_sys_inputs* inputs);
typedef void (*piuio_itg_set_output_pad_t)(uint8_t player, struct piuio_itg_pad_outputs* outputs);
typedef void (*piuio_itg_set_output_cab_t)(const struct piuio_itg_cab_outputs* outputs);

struct piuio_itg_api {
    piuio_itg_open_t open;
    piuio_itg_close_t close;
    piuio_itg_recv_t recv;
    piuio_itg_send_t send;
    piuio_itg_get_input_pad_t get_input_pad;
    piuio_itg_get_input_button_t get_input_button;
    piuio_itg_get_input_sys_t get_input_sys;
    piuio_itg_set_output_pad_t set_output_pad;
    piuio_itg_set_output_cab_t set_output_cab;
};