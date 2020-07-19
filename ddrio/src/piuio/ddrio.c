#include <stdio.h>
#include <string.h>

#include "ddrio/ddrio.h"
#include "piuio/itg/piuio-itg.h"

static struct ddrio_pad_input _ddr_pad_input[DDRIO_PLAYER_NUM];
static struct ddrio_button_input _ddr_button_input[DDRIO_PLAYER_NUM];
static struct ddrio_sys_input _ddr_sys_input;

static struct ddrio_pad_output _ddr_pad_output[DDRIO_PLAYER_NUM];
static struct ddrio_button_output _ddr_button_output[DDRIO_PLAYER_NUM];
static struct ddrio_cabinet_output _ddr_cabinet_output;

static void _merge_sensor_states(
    struct piuio_itg_pad_input* itg_sensor_inputs,
    struct ddrio_pad_input* ddr_input_out)
{
    memset(ddr_input_out, 0, sizeof(struct ddrio_pad_input));

    for (uint8_t i = 0; i < PIUIO_ITG_SENSOR_GROUP_NUM; i++) {
        ddr_input_out->left = ddr_input_out->left || itg_sensor_inputs[i].left;
        ddr_input_out->down = ddr_input_out->down || itg_sensor_inputs[i].down;
        ddr_input_out->up = ddr_input_out->up || itg_sensor_inputs[i].up;
        ddr_input_out->right = ddr_input_out->right || itg_sensor_inputs[i].right;
    }
}

static void _copy_inputs()
{
    struct piuio_itg_pad_input _itg_pad_input[PIUIO_ITG_PLAYER_NUM][PIUIO_ITG_SENSOR_GROUP_NUM];
    struct piuio_itg_button_input _itg_button_input[PIUIO_ITG_PLAYER_NUM];
    struct piuio_itg_sys_input _itg_sys_input;

    for (uint8_t i = 0; i < DDRIO_PLAYER_NUM; i++) {
        for (uint8_t j = 0; j < PIUIO_ITG_SENSOR_GROUP_NUM; j++) {
            piuio_itg_get_input_pad(
                (enum piuio_itg_player) i, 
                (enum piuio_itg_sensor_group) j,
                &_itg_pad_input[i][j]);
        }

        _merge_sensor_states(_itg_pad_input[i], &_ddr_pad_input[i]);

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

bool ddrio_update()
{
    _copy_outputs();

    if (!piuio_itg_send()) {
        return false;
    }

    if (!piuio_itg_recv()) {
        return false;
    }

    _copy_inputs();

    return true;
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

void ddrio_get_pad_output(enum ddrio_player player, struct ddrio_pad_output* output)
{
    memcpy(output, &_ddr_pad_output[player], sizeof(struct ddrio_pad_output));
}

void ddrio_set_pad_output(enum ddrio_player player, const struct ddrio_pad_output* output)
{
    memcpy(&_ddr_pad_output[player], output, sizeof(struct ddrio_pad_output));
}

void ddrio_get_button_output(enum ddrio_player player, struct ddrio_button_output* output)
{
    memcpy(output, &_ddr_button_output[player], sizeof(struct ddrio_button_output));
}

void ddrio_set_button_output(enum ddrio_player player, const struct ddrio_button_output* output)
{
    memcpy(&_ddr_button_output[player], output, sizeof(struct ddrio_button_output));
}

void ddrio_get_cabinet_output(struct ddrio_cabinet_output* output)
{
    memcpy(output, &_ddr_cabinet_output, sizeof(struct ddrio_cabinet_output));
}

void ddrio_set_cabinet_output(const struct ddrio_cabinet_output* output)
{
    memcpy(&_ddr_cabinet_output, output, sizeof(struct ddrio_cabinet_output));
}