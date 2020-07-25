#include <stdio.h>
#include <string.h>

#include "ddrio/ddrio.h"
#include "piuio/itg/piuio-itg.h"

static const enum piuio_itg_sensor_group _ddrio_to_piuio_sensor_group_map[DDRIO_PANEL_SENSOR_NUM] = {
    PIUIO_ITG_SENSOR_GROUP_LEFT,
    PIUIO_ITG_SENSOR_GROUP_RIGHT,
    PIUIO_ITG_SENSOR_GROUP_UP,
    PIUIO_ITG_SENSOR_GROUP_DOWN
};

static struct ddrio_pad_input _ddr_pad_input[DDRIO_PLAYER_NUM];
static struct ddrio_button_input _ddr_button_input[DDRIO_PLAYER_NUM];
static struct ddrio_sys_input _ddr_sys_input;

static enum ddrio_panel_sensor _ddr_panel_sensor[DDRIO_PLAYER_NUM];
static struct ddrio_pad_output _ddr_pad_output[DDRIO_PLAYER_NUM];
static struct ddrio_button_output _ddr_button_output[DDRIO_PLAYER_NUM];
static struct ddrio_cabinet_output _ddr_cabinet_output;

static void _copy_inputs()
{
    struct piuio_itg_pad_input _itg_pad_input[PIUIO_ITG_PLAYER_NUM];
    struct piuio_itg_button_input _itg_button_input[PIUIO_ITG_PLAYER_NUM];
    struct piuio_itg_sys_input _itg_sys_input;

    for (uint8_t i = 0; i < DDRIO_PLAYER_NUM; i++) {
        piuio_itg_get_input_pad( (enum piuio_itg_player) i, &_itg_pad_input[i]);

        _ddr_pad_input[i].left = _itg_pad_input[i].left;
        _ddr_pad_input[i].down = _itg_pad_input[i].down;
        _ddr_pad_input[i].up = _itg_pad_input[i].up;
        _ddr_pad_input[i].right = _itg_pad_input[i].right;

        piuio_itg_get_input_button((enum piuio_itg_player) i, &_itg_button_input[i]);

        _ddr_button_input[i].left = _itg_button_input[i].left;
        _ddr_button_input[i].right = _itg_button_input[i].right;
        _ddr_button_input[i].start = _itg_button_input[i].start;
    }

    piuio_itg_get_input_sys(&_itg_sys_input);

    _ddr_sys_input.test = _itg_sys_input.test;
    _ddr_sys_input.service = _itg_sys_input.service;
    _ddr_sys_input.coin1 = _itg_sys_input.coin;
    _ddr_sys_input.coin2 = _itg_sys_input.coin2;
}

static void _copy_outputs()
{
    struct piuio_itg_pad_output itg_pad_output[PIUIO_ITG_PLAYER_NUM];
    struct piuio_itg_cabinet_output itg_cabinet_output;

    for (uint8_t i = 0; i < DDRIO_PLAYER_NUM; i++) {
        piuio_itg_set_output_pad_sensor_group(
            (enum piuio_itg_player) i, 
            _ddrio_to_piuio_sensor_group_map[_ddr_panel_sensor[i]]);

        itg_pad_output[i].left = _ddr_pad_output[i].left;
        itg_pad_output[i].down = _ddr_pad_output[i].down;
        itg_pad_output[i].up = _ddr_pad_output[i].up;
        itg_pad_output[i].right = _ddr_pad_output[i].right;

        piuio_itg_set_output_pad((enum piuio_itg_player) i, &itg_pad_output[i]);
    }

    itg_cabinet_output.bass = _ddr_cabinet_output.bass;
    itg_cabinet_output.halo_r2 = _ddr_cabinet_output.lamp_r2;
    itg_cabinet_output.halo_r1 = _ddr_cabinet_output.lamp_r1;
    itg_cabinet_output.halo_l2 = _ddr_cabinet_output.lamp_l2;
    itg_cabinet_output.halo_l1 = _ddr_cabinet_output.lamp_l1;

    piuio_itg_set_output_cabinet(&itg_cabinet_output);
}

const char* ddrio_ident()
{
    return "PIUIO";
}

bool ddrio_open()
{
    return piuio_itg_open();
}

bool ddrio_close()
{
    piuio_itg_close();

    return true;
}

bool ddrio_read_input()
{
    if (!piuio_itg_recv()) {
        return false;
    }

    _copy_inputs();

    return true;
}

bool ddrio_write_output()
{
    _copy_outputs();

    return piuio_itg_send();
}

void ddrio_get_pad_input(enum ddrio_player player, struct ddrio_pad_input* input)
{
    memcpy(input, &_ddr_pad_input[player], sizeof(struct ddrio_pad_input));
}

void ddrio_get_button_input(enum ddrio_player player, struct ddrio_button_input* input)
{
    memcpy(input, &_ddr_button_input[player], sizeof(struct ddrio_button_input));
}

void ddrio_get_sys_input(struct ddrio_sys_input* input)
{
    memcpy(input, &_ddr_sys_input, sizeof(struct ddrio_sys_input));
}

void ddrio_set_pad_panel_sensor(enum ddrio_player player, enum ddrio_panel_sensor sensor)
{
    _ddr_panel_sensor[player] = sensor;
}

void ddrio_set_pad_output(enum ddrio_player player, const struct ddrio_pad_output* output)
{
    memcpy(&_ddr_pad_output[player], output, sizeof(struct ddrio_pad_output));
}

void ddrio_set_button_output(enum ddrio_player player, const struct ddrio_button_output* output)
{
    memcpy(&_ddr_button_output[player], output, sizeof(struct ddrio_button_output));
}

void ddrio_set_cabinet_output(const struct ddrio_cabinet_output* output)
{
    memcpy(&_ddr_cabinet_output, output, sizeof(struct ddrio_cabinet_output));
}