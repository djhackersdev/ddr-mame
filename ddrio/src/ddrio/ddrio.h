/**
 * DDRIO API
 * 
 * This header file provides an interface to write your own implementations for any kind of custom
 * hardware to interface with the game. All inputs and outputs are covered which gives you the
 * option to hook up hardware covering all aspects of the original cabinets. However, you can also
 * just implement input handling and just stub output handling for pure input based hardware/setups.
 * 
 * This interface is purely synchronous and does not have to be thread safe. An additional backend
 * driver layer takes care of polling the hardware asynchronously and buffer management. Do not
 * implement your own threaded backend as it will not allow you to gain more performance and just
 * increases the complexity of your implementation.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

enum ddrio_player {
    DDRIO_PLAYER_1 = 0,
    DDRIO_PLAYER_2 = 1,
    DDRIO_PLAYER_NUM = 2,
};

enum ddrio_panel_sensor {
    DDRIO_PANEL_SENSOR_LEFT = 0,
    DDRIO_PANEL_SENSOR_RIGHT = 1,
    DDRIO_PANEL_SENSOR_UP = 2,
    DDRIO_PANEL_SENSOR_DOWN = 3,
    DDRIO_PANEL_SENSOR_NUM = 4,
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
typedef bool (*ddrio_read_input_t)();
typedef bool (*ddrio_write_output_t)();
typedef void (*ddrio_get_pad_input_t)(enum ddrio_player player, struct ddrio_pad_input* input);
typedef void (*ddrio_get_button_input_t)(enum ddrio_player player, struct ddrio_button_input* input);
typedef void (*ddrio_get_sys_input_t)(struct ddrio_sys_input* input);
typedef void (*ddrio_set_pad_panel_sensor_t)(enum ddrio_player player, enum ddrio_panel_sensor sensor);
typedef void (*ddrio_set_pad_output_t)(enum ddrio_player player, const struct ddrio_pad_output* output);
typedef void (*ddrio_set_button_output_t)(enum ddrio_player player, const struct ddrio_button_output* output);
typedef void (*ddrio_set_cabinet_output_t)(const struct ddrio_cabinet_output* output);

/**
 * Structure for exposing the entire API dynamically.
 */
struct ddrio_api {
    bool initialized;
    ddrio_ident_t ident;
    ddrio_open_t open;
    ddrio_close_t close;
    ddrio_read_input_t read_input;
    ddrio_write_output_t write_output;
    ddrio_get_pad_input_t get_pad_input;
    ddrio_get_button_input_t get_button_input;
    ddrio_get_sys_input_t get_sys_input;
    ddrio_set_pad_panel_sensor_t set_pad_panel_sensor;
    ddrio_set_pad_output_t set_pad_output;
    ddrio_set_button_output_t set_button_output;
    ddrio_set_cabinet_output_t set_cabinet_output;
};

/**
 * Get a human readable identifier for the implementation of the API.
 * 
 * Only used for debugging purpose when the API is initialized to know which API was used.
 */
const char* ddrio_ident();

/**
 * Open the device.
 * 
 * Open and connect to the hardware, execute any additional initialization routines and prepare
 * any buffers to be used.
 * 
 * @return True if successful, false on any error.
 */
bool ddrio_open();

/**
 * Close the device.
 * 
 * Shutdown hardware, cleanup handles and buffers.
 */
bool ddrio_close();

/**
 * Execute a synchronous poll of the hardware to read new input data from it.
 * 
 * Make this request data from your device and buffer it internally.
 * 
 * @return True if reading input data was successful, false on any errors.
 */
bool ddrio_read_input();

/**
 * Execute a synchronous poll of the hardware to write output data to it.
 * 
 * This step might be optional if you can combine writing output data with reading request data
 * in ddrio_read_input. In that case, just leave this function empty.
 * 
 * Otherwise, send a request to write any bufferd output data to your hardware.
 */
bool ddrio_write_output();

/**
 * Get buffered input data of the platforms since the last call to ddrio_read_input.
 * 
 * @param player The player side
 * @param input Pointer to a buffer to copy the buffered inputs to.
 */
void ddrio_get_pad_input(enum ddrio_player player, struct ddrio_pad_input* input);

/**
 * Get buffered input data of the cabinet menu buttons since the last call to ddrio_read_input.
 * 
 * @param player The player side
 * @param input Pointer to a buffer to copy the buffered inputs to.
 */
void ddrio_get_button_input(enum ddrio_player player, struct ddrio_button_input* input);

/**
 * Get buffered input data of the system/operator inputs, e.g. service, test, coin.
 * 
 * @param input Pointer to a buffer to copy the buffered inputs to.
 */
void ddrio_get_sys_input(struct ddrio_sys_input* input);

/**
 * Set the sensor data to be written on the next call to ddrio_write_output and inputs to be read
 * back on the next call to ddrio_read-input.
 * 
 * @param player The player side
 * @param sensor The sensor to be read on the next input poll
 */
void ddrio_set_pad_panel_sensor(enum ddrio_player player, enum ddrio_panel_sensor sensor);

/**
 * Set the output data for the platform lights to be written on the next call to ddrio_write_output
 * 
 * @param player The player side
 * @param output The output data to be written
 */
void ddrio_set_pad_output(enum ddrio_player player, const struct ddrio_pad_output* output);

/**
 * Set the output data for cabinet menu button lights to be written on the next call to
 * ddrio_write_output
 * 
 * @param player The player side
 * @param output The output data to be written
 */
void ddrio_set_button_output(enum ddrio_player player, const struct ddrio_button_output* output);

/**
 * Set the output data for cabinet lights to be written on the next call to ddrio_write_output
 * 
 * @param output The output data to be written
 */
void ddrio_set_cabinet_output(const struct ddrio_cabinet_output* output);