#pragma once

#include <atomic>
#include <mutex>
#include <stdint.h>

#include "ddrio.h"

#define DDRIO_DRIVER_POLL_RATE_NO_LIMIT 0

/**
 * Backend driving an implementation of the DDRIO interface
 *
 * The backend is fully threaded to de-couple polling from the game's input reading loop and
 * avoiding high latencies that slow the game loop down. On real hardware, the stage IO takes care
 * of polling the four sensores of each panel asynchronous. During gameplay, a merged view is on a
 * full panel is provided as a single input over the JAMMA connector.
 * On the operator menu's IO test screen, a slow serial protocol is used to address each sensor to
 * allow displaying indivudual states on screen for hardware debugging.
 * This backend mimics that behavior.
 * Furthermore, the polling thread can be configured to a fixed polling rate avoiding high CPU load
 * if the input latency is already good enough and a further increase in polling frequency won't
 * improve it.
 * Double buffering of inputs and outputs with an atomic swap of front and back ensures that no
 * significant latency is exposed to the ksys573 module.
 */
class ddrio_driver
{
public:
    enum pad_panel_sensor {
        PAD_PANEL_SENSOR_LEFT = 0,
        PAD_PANEL_SENSOR_RIGHT = 1,
        PAD_PANEL_SENSOR_UP = 2,
        PAD_PANEL_SENSOR_DOWN = 3,
        PAD_PANEL_SENSOR_ALL = 4
    };

    struct input_state {
        struct pad {
            bool left;
            bool down;
            bool up;
            bool right; 
        } pad[2];

        struct cabinet_player {
            bool left;
            bool right;
            bool start;
        } cabinet_player[2];

        struct cabinet_operator {
            bool test;
            bool service;
            bool coin1;
            bool coin2;
        } cabinet_operator;
    };

    struct output_state {
        enum pad_panel_sensor pad_panel_sensor[2];

        struct pad_light {
            bool left;
            bool down;
            bool up;
            bool right; 
        } pad_light[2];

        struct cabinet_player {
            bool left;
            bool right;
            bool start;
        } cabinet_player[2];

        struct cabinet {
            bool lamp_r2;
            bool lamp_r1;
            bool lamp_l2;
            bool lamp_l1;
            bool bass;
        } cabinet;
    };

    /**
     * Constructor
     * 
     * @param poll_rate_hz The max poll rate for the backend thread to poll the hardware. Limit this
     *                     to avoid high CPU load. Polling the hardware more frequent than the
     *                     game's update loop doesn't bring any benefit regarding accuracy.
     */
    ddrio_driver(uint16_t poll_rate_hz);

    /**
     * Initialize the driver.
     * 
     * Resets buffers and state, and starts the backend thread for driving the hardware of the
     * DDRIO API implementation.
     *
     * @return True if successful, false when loading the DDRIO API implementation or opening the
     *         DDRIO device fails.
     */
    bool init();

    /**
     * Shutdown the driver.
     * 
     * Stop the backend thread and close the hardware of the DDRIO API implementation.
     */
    void shutdown();

    /**
     * Get a pointer to the currently exposed buffered inputs.
     */
    const struct input_state* input_state_front() {
        return m_input_state_front;
    }

    /**
     * Get a pointer to the buffer for writing outputs.
     */
    struct output_state* output_state_front() {
        return m_output_state_front;
    }

    /**
     * Helper to easily setting the output buffer that the backend polls for all sensores of each
     * panel and merging the inputs into a single one.
     */
    void output_state_front_panel_sensor_all();

    /**
     * Helper for switching off all lights in the output buffer.
     */
    void output_state_front_lights_off();

    /**
     * Execute a swap of the input front and back buffers.
     * 
     * Call this every time before reading inputs from the front buffer, to get the latest updates
     * from the polling backend.
     */
    void input_state_swap();

    /**
     * Execute a swap of the output front and back buffers.
     * 
     * Call this after you set the output states in the front buffer to propagte them to the polling
     * backend that applies them to the hardware.
     */
    void output_state_swap();

private:
    uint16_t m_poll_rate_hz;

    struct ddrio_api m_ddrio_api;

    std::mutex m_input_state_swap_mutex;
    bool m_input_states_updated;
    struct input_state m_input_state[2];
    struct input_state* m_input_state_front;
    struct input_state* m_input_state_back;

    std::mutex m_output_state_swap_mutex;
    // Used for cycling sensors while polling when the game wants all of them merged
    uint8_t m_current_sensor[2];
    struct output_state m_output_state[2];
    struct output_state* m_output_state_front;
    struct output_state* m_output_state_back;

    std::thread* m_io_thread;
    std::atomic<bool> m_io_thread_run;

    void _reset_state_buffers();

    void _start_io_thread();
    void _stop_io_thread();
    void _io_thread_main();

    void _copy_from_output_state_back(struct output_state* state);
    void _copy_to_input_state_back(const struct input_state* state);
    void _ddrio_write_outputs(const struct output_state* output_state);
    void _ddrio_read_inputs(struct input_state* input_state);
    void _merge_input_states(
        struct input_state* merged,
        struct input_state* input_states,
        uint32_t count);
    void _rate_limit_and_sleep(std::size_t poll_elapsed_time_us);
};