#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ddrio/ddrio.h"

static struct ddrio_pad_input _ddr_pad_input[DDRIO_PLAYER_NUM];
static struct ddrio_button_input _ddr_button_input[DDRIO_PLAYER_NUM];
static struct ddrio_sys_input _ddr_sys_input;

static struct ddrio_pad_output _ddr_pad_output[DDRIO_PLAYER_NUM];
static struct ddrio_button_output _ddr_button_output[DDRIO_PLAYER_NUM];
static struct ddrio_cabinet_output _ddr_cabinet_output;

const char* ddrio_ident()
{
    printf("ddrio_ident\n");

    return "Debug";
}

bool ddrio_open()
{
    printf("ddrio_open\n");
    return true;
}

bool ddrio_close()
{
    printf("ddrio_close\n");

    return true;
}

bool ddrio_read_input()
{
    memset(&_ddr_pad_input[DDRIO_PLAYER_1], 0 , sizeof(struct ddrio_pad_input));
    memset(&_ddr_pad_input[DDRIO_PLAYER_2], 0 , sizeof(struct ddrio_pad_input));

    memset(&_ddr_button_input[DDRIO_PLAYER_1], 0 , sizeof(struct ddrio_button_input));
    memset(&_ddr_button_input[DDRIO_PLAYER_2], 0 , sizeof(struct ddrio_button_input));

    memset(&_ddr_sys_input, 0 , sizeof(struct ddrio_sys_input));

    FILE* file = fopen("/tmp/ddrio", "r");

    if (file) {
        int values[18];

        fseek(file, 0, SEEK_END);
        int size = ftell(file);
        fseek(file, 0, SEEK_SET);

        char* buffer = malloc(size);
        fread(buffer, size, 1, file);
        fclose(file);

        sscanf(buffer, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
            &values[0],
            &values[1],
            &values[2],
            &values[3],
            &values[4],
            &values[5],
            &values[6],
            &values[7],
            &values[8],
            &values[9],
            &values[10],
            &values[11],
            &values[12],
            &values[13],
            &values[14],
            &values[15],
            &values[16],
            &values[17]);

            _ddr_pad_input[DDRIO_PLAYER_1].left = values[0] > 0;
            _ddr_pad_input[DDRIO_PLAYER_1].down = values[1] > 0;
            _ddr_pad_input[DDRIO_PLAYER_1].up = values[2] > 0;
            _ddr_pad_input[DDRIO_PLAYER_1].right = values[3] > 0;
            _ddr_pad_input[DDRIO_PLAYER_2].left = values[4] > 0;
            _ddr_pad_input[DDRIO_PLAYER_2].down = values[5] > 0;
            _ddr_pad_input[DDRIO_PLAYER_2].up = values[6] > 0;
            _ddr_pad_input[DDRIO_PLAYER_2].right = values[7] > 0;
            _ddr_button_input[DDRIO_PLAYER_1].left = values[8] > 0;
            _ddr_button_input[DDRIO_PLAYER_1].right = values[9] > 0;
            _ddr_button_input[DDRIO_PLAYER_1].start = values[10] > 0;
            _ddr_button_input[DDRIO_PLAYER_2].left = values[11] > 0;
            _ddr_button_input[DDRIO_PLAYER_2].right = values[12] > 0;
            _ddr_button_input[DDRIO_PLAYER_2].start = values[13] > 0;
            _ddr_sys_input.test = values[14] > 0;
            _ddr_sys_input.service = values[15] > 0;
            _ddr_sys_input.coin1 = values[16] > 0;
            _ddr_sys_input.coin2 = values[17] > 0;
    }

    return true;
}

bool ddrio_write_output()
{
    // TODO cabinet output if not 0

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

void ddrio_set_pad_panel_sensor(enum ddrio_player player, enum ddrio_panel_sensor sensor)
{
    // empty
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