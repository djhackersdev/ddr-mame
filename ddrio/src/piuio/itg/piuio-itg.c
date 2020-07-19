/**
 * Implementation of the piuio API. This implements a piuio usb driver to
 * communicate with real piuio hardware and also shares a common interface with
 * other "piuio devices" (e.g. keyboard, custom IOs)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "piuio/device/device.h"
#include "piuio/itg/piuio-itg.h"

#define BUFFER_SIZE 8

static uint8_t _in_buffer[BUFFER_SIZE];
static uint8_t _out_buffer[BUFFER_SIZE];

static struct piuio_itg_pad_input _pad_input[PIUIO_ITG_PLAYER_NUM][PIUIO_ITG_SENSOR_GROUP_NUM];
static struct piuio_itg_button_input _button_input[PIUIO_ITG_PLAYER_NUM];
static struct piuio_itg_sys_input _sys_input;
static struct piuio_itg_pad_output _pad_output[PIUIO_ITG_PLAYER_NUM];
static struct piuio_itg_cabinet_output _cabinet_output;

static void _copy_outputs(uint8_t sensor_group)
{
    memset(_out_buffer, 0, sizeof(_out_buffer));

	/* 00 = sensor 1 (right), 01 = sensor 2 (left), 10 = sensor 3 (down),
       11 = sensor 4 (up) */
    /* Pad 1 sensor group selector */
    _out_buffer[0] |= sensor_group;
    /* Pad 2 sensor group selector */
    _out_buffer[2] |= sensor_group;

    /* Set pad lights */
    for (uint8_t i = 0; i < PIUIO_ITG_PLAYER_NUM; i++) {
        if (_pad_output[i].up) {
            _out_buffer[i * 2] |= (1 << 2);
        }

        if (_pad_output[i].down) {
            _out_buffer[i * 2] |= (1 << 3);
        }

        if (_pad_output[i].left) {
            _out_buffer[i * 2] |= (1 << 4);
        }

        if (_pad_output[i].right) {
            _out_buffer[i * 2] |= (1 << 5);
        }
    }

    /* Neons/Bass */
    if (_cabinet_output.bass) {
        _out_buffer[1] |= (1 << 2);
    }

    /* Halogens */
    if (_cabinet_output.halo_l1) {
        _out_buffer[2] |= (1 << 7);
    }

    if (_cabinet_output.halo_r1) {
        _out_buffer[3] |= (1 << 0);
    }

    if (_cabinet_output.halo_l2) {
        _out_buffer[3] |= (1 << 1);
    }

    if (_cabinet_output.halo_r2) {
        _out_buffer[3] |= (1 << 2);
    }

	// // set coin counter 1
	// _out_buffer[3] |= ((m_outputStates->m_counter & 0x01) << 4);

	// // set coin counter 2 (front usb enable)
	// _out_buffer[1] |= ((m_outputStates->m_counter & 0x02) << 3);
}

static void _copy_inputs(uint8_t sensor_group)
{
    for (uint8_t i = 0; i < PIUIO_ITG_PLAYER_NUM; i++) {
        _pad_input[i][sensor_group].up = _in_buffer[i * 2] & (1 << 0);
        _pad_input[i][sensor_group].down = _in_buffer[i * 2] & (1 << 1);
        _pad_input[i][sensor_group].left = _in_buffer[i * 2] & (1 << 2);
        _pad_input[i][sensor_group].right = _in_buffer[i * 2] & (1 << 3);

        _button_input[i].start = _in_buffer[i * 2] & (1 << 4);
        _button_input[i].back = _in_buffer[i * 2] & (1 << 5);
        _button_input[i].left = _in_buffer[i * 2] & (1 << 6);
        _button_input[i].right = _in_buffer[i * 2] & (1 << 7);
    }

    /* Sys */
    _sys_input.test = _in_buffer[1] & (1 << 1);
    _sys_input.service = _in_buffer[1] & (1 << 6);
    _sys_input.clear = _in_buffer[1] & (1 << 7);
    _sys_input.coin = _in_buffer[1] & (1 << 2);

    memset(_in_buffer, 0, sizeof(_in_buffer));
}

bool piuio_itg_open(void)
{
    return piuio_device_open();
}

void piuio_itg_close(void)
{
    piuio_device_close();
}

bool piuio_itg_recv(void)
{
    /* cycle all four sensor groups */
    for (uint8_t i = 0; i < PIUIO_ITG_SENSOR_GROUP_NUM; i++) {
        _copy_outputs(i);

        if (!piuio_device_write(_out_buffer, sizeof(_out_buffer))) {
            return false;
        }

        if (!piuio_device_read(_in_buffer, sizeof(_in_buffer))) {
            return false;
        }

        /* Invert pull ups */
        for (uint8_t j = 0; j < 4; j++) {
            _in_buffer[j] ^= 0xFF;
        }

        _copy_inputs(i);
    }

    return true;
}

bool piuio_itg_send(void)
{
    /* Already done in recv */
    return true;
}

void piuio_itg_get_input_pad(
        enum piuio_itg_player player,
        enum piuio_itg_sensor_group sensor_group,
        struct piuio_itg_pad_input* input)
{
    memcpy(input, &_pad_input[player][sensor_group], sizeof(struct piuio_itg_pad_input));
}

void piuio_itg_get_input_sys(struct piuio_itg_sys_input* input)
{
    memcpy(input, &_sys_input, sizeof(struct piuio_itg_sys_input));
}

void piuio_itg_get_input_button(
        enum piuio_itg_player player,
        struct piuio_itg_button_input* input)
{
    memcpy(input, &_button_input[player], sizeof(struct piuio_itg_button_input));
}

void piuio_itg_set_output_pad(
        enum piuio_itg_player player,
        const struct piuio_itg_pad_output* output)
{
    memcpy(&_pad_output[player], output, sizeof(struct piuio_itg_pad_output));
}

void piuio_itg_set_output_cabinet(
        const struct piuio_itg_cabinet_output* output)
{
    memcpy(&_cabinet_output, output, sizeof(struct piuio_itg_cabinet_output));
}