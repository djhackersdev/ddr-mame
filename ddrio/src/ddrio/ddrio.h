#pragma once

#include <stdbool.h>
#include <stdint.h>

enum ddrio_player {
    DDRIO_PLAYER_1 = 0,
    DDRIO_PLAYER_2 = 1,
    DDRIO_PLAYER_NUM = 2,
};

struct ddrio_pad_input {
    bool left;
    bool down;
    bool up;
    bool right;
};

struct ddrio_button_input {
    bool left;
    bool right;
    bool start;
};

struct ddrio_sys_input {
    bool test;
    bool service;
    bool coin1;
    bool coin2;
};

struct ddrio_pad_output {
    bool left;
    bool down;
    bool up;
    bool right;
};

struct ddrio_button_output {
    bool left;
    bool right;
    bool start;
};

struct ddrio_cabinet_output {
    bool lamp_r2;
    bool lamp_r1;
    bool lamp_l2;
    bool lamp_l1;
    bool bass;
};

typedef const char* (*ddrio_ident_t)();
typedef bool (*ddrio_open_t)();
typedef bool (*ddrio_close_t)();
typedef bool (*ddrio_update_t)();
typedef void (*ddrio_get_pad_input_t)(enum ddrio_player player, struct ddrio_pad_input* input);
typedef void (*ddrio_get_button_input_t)(enum ddrio_player player, struct ddrio_button_input* input);
typedef void (*ddrio_get_sys_input_t)(struct ddrio_sys_input* input);
typedef void (*ddrio_get_pad_output_t)(enum ddrio_player player, struct ddrio_pad_output* output);
typedef void (*ddrio_set_pad_output_t)(enum ddrio_player player, const struct ddrio_pad_output* output);
typedef void (*ddrio_get_button_output_t)(enum ddrio_player player, struct ddrio_button_output* output);
typedef void (*ddrio_set_button_output_t)(enum ddrio_player player, const struct ddrio_button_output* output);
typedef void (*ddrio_get_cabinet_output_t)(struct ddrio_cabinet_output* output);
typedef void (*ddrio_set_cabinet_output_t)(const struct ddrio_cabinet_output* output);

struct ddrio_api {
    bool initialized;
    ddrio_ident_t ident;
    ddrio_open_t open;
    ddrio_close_t close;
    ddrio_update_t update;
    ddrio_get_pad_input_t get_pad_input;
    ddrio_get_button_input_t get_button_input;
    ddrio_get_sys_input_t get_sys_input;
    ddrio_get_pad_output_t get_pad_output;
    ddrio_set_pad_output_t set_pad_output;
    ddrio_get_button_output_t get_button_output;
    ddrio_set_button_output_t set_button_output;
    ddrio_get_cabinet_output_t get_cabinet_output;
    ddrio_set_cabinet_output_t set_cabinet_output;
};

const char* ddrio_ident();

bool ddrio_open();

bool ddrio_close();

bool ddrio_update();

void ddrio_get_pad_input(enum ddrio_player player, struct ddrio_pad_input* input);

void ddrio_get_button_input(enum ddrio_player player, struct ddrio_button_input* input);

void ddrio_get_sys_input(struct ddrio_sys_input* input);

void ddrio_get_pad_output(enum ddrio_player player, struct ddrio_pad_output* output);

void ddrio_set_pad_output(enum ddrio_player player, const struct ddrio_pad_output* output);

void ddrio_get_button_output(enum ddrio_player player, struct ddrio_button_output* output);

void ddrio_set_button_output(enum ddrio_player player, const struct ddrio_button_output* output);

void ddrio_get_cabinet_output(struct ddrio_cabinet_output* output);

void ddrio_set_cabinet_output(const struct ddrio_cabinet_output* output);