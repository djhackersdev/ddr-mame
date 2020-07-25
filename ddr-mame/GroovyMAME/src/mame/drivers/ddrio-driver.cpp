#include <chrono>
#include <thread>
#include <stdio.h>
#include <string.h>

#include "ddrio-api.h"
#include "ddrio-driver.h"

#define DDR_PAD_PANEL_SENSOR_COUNT 4

ddrio_driver::ddrio_driver(uint16_t poll_rate_hz) :
    m_poll_rate_hz(poll_rate_hz),
    m_input_state_swap_mutex(),
    m_input_states_updated(false),
    m_input_state_front(nullptr),
    m_input_state_back(nullptr),
    m_output_state_swap_mutex(),
    m_output_state_front(nullptr),
    m_output_state_back(nullptr),
    m_io_thread(nullptr),
    m_io_thread_run(false)
{
    memset(&m_ddrio_api, 0, sizeof(struct ddrio_api));

    memset(&m_input_state, 0, sizeof(struct input_state) * 2);
    memset(&m_current_sensor, 0, sizeof(uint8_t) * 2);
    memset(&m_output_state, 0, sizeof(struct output_state) * 2);
}

bool ddrio_driver::init()
{
    printf("Initializing DDRIO driver...\n");

    _reset_state_buffers();

	// Assuming that the working directory is next to the mame executable
	if (!ddrio_api_load("./ddrio.so", &m_ddrio_api)) {
		printf("ERROR: Loading DDRIO library\n");
        return false;
	} else {
		printf("Initializing DDRIO implementation: %s\n", m_ddrio_api.ident());

		if (!m_ddrio_api.open()) {
			printf("ERROR: Opening DDRIO failed\n");
			memset(&m_ddrio_api, 0, sizeof(struct ddrio_api));
            return false;
		}

		printf("DDRIO opened\n");

        _start_io_thread();

        return true;
	}
}

void ddrio_driver::shutdown()
{
    _stop_io_thread();

    printf("Closing DDRIO driver...\n");

    m_ddrio_api.close();
}

void ddrio_driver::output_state_front_panel_sensor_all()
{
	m_output_state_front->pad_panel_sensor[0] = ddrio_driver::PAD_PANEL_SENSOR_ALL;
	m_output_state_front->pad_panel_sensor[1] = ddrio_driver::PAD_PANEL_SENSOR_ALL;
}

void ddrio_driver::output_state_front_lights_off()
{
    memset(&m_output_state_front->pad_light, 0, sizeof(struct output_state::pad_light) * 2);
    memset(&m_output_state_front->cabinet_player, 0, sizeof(struct output_state::cabinet_player) * 2);
    memset(&m_output_state_front->cabinet, 0, sizeof(struct output_state::cabinet));
}

void ddrio_driver::input_state_swap()
{
    m_input_state_swap_mutex.lock();

    // Only excecute swapping if new data is available. Otherwise, if the application is polling a
    // lot faster than the backend can update, e.g. due to "slower" IO or rate limited update
    // frequencey, the input states will jitter between the currently updated and previous state
    // which can be anything. This results in random on/off patterns for the inputs. Therefore,
    // if the backend could not update the back buffer, yet, keep the front buffer and consider that
    // the current state reflected by the hardware.
    // If the inputs delivered to the game start to feel "sticky" and less responsive on high
    // frequent button mashing, consider increasing the refresh rate of the device, if possible.
    // Otherwise, your IO hardware might be too slow or not optimized well enough.
    if (m_input_states_updated) {
        struct input_state* tmp = m_input_state_front;
        m_input_state_front = m_input_state_back;
        m_input_state_back = tmp;

        m_input_states_updated = false;
    }

    m_input_state_swap_mutex.unlock();
}

void ddrio_driver::output_state_swap()
{
    m_output_state_swap_mutex.lock();

    struct output_state* tmp = m_output_state_front;
    m_output_state_front = m_output_state_back;
    m_output_state_back = tmp;

    m_output_state_swap_mutex.unlock();

    memset(m_output_state_front, 0, sizeof(struct output_state));
}

void ddrio_driver::_reset_state_buffers()
{
    memset(&m_current_sensor, 0, sizeof(uint8_t) * 2);

    memset(&m_input_state, 0, sizeof(struct input_state) * 2);
    memset(&m_output_state, 0, sizeof(struct output_state) * 2);

    m_input_state_front = &m_input_state[0];
    m_input_state_back = &m_input_state[1];

    m_output_state_front = &m_output_state[0];
    m_output_state_back = &m_output_state[1];

    m_input_states_updated = false;
}

void ddrio_driver::_start_io_thread()
{
    printf("Starting io thread...\n");

    m_io_thread_run.store(true);
    m_io_thread = new std::thread(&ddrio_driver::_io_thread_main, this);
}

void ddrio_driver::_stop_io_thread()
{
    printf("Stopping io thread...\n");

    m_io_thread_run.store(true);
    m_io_thread->join();
    delete m_io_thread;
    m_io_thread = nullptr;
}

void ddrio_driver::_io_thread_main()
{
    printf("Started io thread\n");

    while (m_io_thread_run.load()) {
        struct input_state input_state;
        struct output_state output_state;

        auto start_time = std::chrono::high_resolution_clock::now();

        memset(&input_state, 0, sizeof(struct input_state));
        memset(&output_state, 0, sizeof(struct output_state));

        _copy_from_output_state_back(&output_state);

        // The game has two modes for reading input data from the pads
        // 1. Merged view on all sensors. This mode is used during gameplay as the stage IO
        // hardware takes care of polling all sensors of each panel fast enough
        // 2. Addressing single sensors. This is only used in the operator menu on the IO test
        // screen to debug broken sensors
        if (output_state.pad_panel_sensor[0] == PAD_PANEL_SENSOR_ALL 
                || output_state.pad_panel_sensor[1] == PAD_PANEL_SENSOR_ALL) {
            struct input_state input_states_per_sensor[DDR_PAD_PANEL_SENSOR_COUNT];

            for (uint8_t i = 0; i < DDR_PAD_PANEL_SENSOR_COUNT; i++) {
                _ddrio_write_outputs(&output_state);

                _ddrio_read_inputs(&input_states_per_sensor[i]);
            }

            _merge_input_states(&input_state, input_states_per_sensor, DDR_PAD_PANEL_SENSOR_COUNT);
        } else {
            // Single sensor polling mode
            _ddrio_write_outputs(&output_state);

            _ddrio_read_inputs(&input_state);
        }

        _copy_to_input_state_back(&input_state);

        auto end_time = std::chrono::high_resolution_clock::now();

        std::size_t poll_elapsed_time_us =
            std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

        _rate_limit_and_sleep(poll_elapsed_time_us);
    }

    printf("Stopped io thread\n");
}

void ddrio_driver::_copy_from_output_state_back(struct output_state* state)
{
    m_output_state_swap_mutex.lock();
    memcpy(state, m_output_state_back, sizeof(struct output_state));
    m_output_state_swap_mutex.unlock();
}

void ddrio_driver::_copy_to_input_state_back(const struct input_state* state)
{
    m_input_state_swap_mutex.lock();
    memcpy(m_input_state_back, state, sizeof(struct input_state));
    m_input_states_updated = true;
    m_input_state_swap_mutex.unlock();
}

void ddrio_driver::_ddrio_write_outputs(const struct output_state* output_state)
{
    struct ddrio_pad_output pad_output[DDRIO_PLAYER_NUM];
	struct ddrio_button_output button_output[DDRIO_PLAYER_NUM];
	struct ddrio_cabinet_output cabinet_output;

    for (uint8_t i = 0; i < DDRIO_PLAYER_NUM; i++) {
        switch (output_state->pad_panel_sensor[i]) {
            case PAD_PANEL_SENSOR_LEFT:
                m_ddrio_api.set_pad_panel_sensor((enum ddrio_player) i, DDRIO_PANEL_SENSOR_LEFT);
                break;
            case PAD_PANEL_SENSOR_RIGHT:
                m_ddrio_api.set_pad_panel_sensor((enum ddrio_player) i, DDRIO_PANEL_SENSOR_RIGHT);
                break;
            case PAD_PANEL_SENSOR_UP:
                m_ddrio_api.set_pad_panel_sensor((enum ddrio_player) i, DDRIO_PANEL_SENSOR_UP);
                break;
            case PAD_PANEL_SENSOR_DOWN:
                m_ddrio_api.set_pad_panel_sensor((enum ddrio_player) i, DDRIO_PANEL_SENSOR_DOWN);
                break;
            case PAD_PANEL_SENSOR_ALL:
            default:
                m_ddrio_api.set_pad_panel_sensor(
                    (enum ddrio_player) i, 
                    (enum ddrio_panel_sensor) m_current_sensor[i]);

                m_current_sensor[i]++;

                // Cycle sensors of panel
                if (m_current_sensor[i] >= DDR_PAD_PANEL_SENSOR_COUNT) {
                    m_current_sensor[i] = 0;
                }

                break;
        }

        pad_output[i].left = output_state->pad_light[i].left;
        pad_output[i].down = output_state->pad_light[i].down;
        pad_output[i].up = output_state->pad_light[i].up;
        pad_output[i].right = output_state->pad_light[i].right;

        m_ddrio_api.set_pad_output((enum ddrio_player) i, &pad_output[i]);

        button_output[i].left = output_state->cabinet_player[i].left;
        button_output[i].right = output_state->cabinet_player[i].right;
        button_output[i].start = output_state->cabinet_player[i].start;

        m_ddrio_api.set_button_output((enum ddrio_player) i, &button_output[i]);
    }

    cabinet_output.lamp_r2 = output_state->cabinet.lamp_r2;
    cabinet_output.lamp_r1 = output_state->cabinet.lamp_r1;
    cabinet_output.lamp_l2 = output_state->cabinet.lamp_l2;
    cabinet_output.lamp_l1 = output_state->cabinet.lamp_l1;
    cabinet_output.bass = output_state->cabinet.bass;

    m_ddrio_api.set_cabinet_output(&cabinet_output);

    m_ddrio_api.write_output();
}

void ddrio_driver::_ddrio_read_inputs(struct input_state* input_state)
{
	struct ddrio_pad_input pad_input[DDRIO_PLAYER_NUM];
	struct ddrio_button_input button_input[DDRIO_PLAYER_NUM];
    struct ddrio_sys_input sys_input;

    m_ddrio_api.read_input();

    for (uint8_t i = 0; i < DDRIO_PLAYER_NUM; i++) {
        m_ddrio_api.get_pad_input((enum ddrio_player) i, &pad_input[i]);
    
        input_state->pad[i].left = pad_input[i].left;
        input_state->pad[i].down = pad_input[i].down;
        input_state->pad[i].up = pad_input[i].up;
        input_state->pad[i].right = pad_input[i].right;

        m_ddrio_api.get_button_input((enum ddrio_player) i, &button_input[i]);

        input_state->cabinet_player[i].left = button_input[i].left;
        input_state->cabinet_player[i].right = button_input[i].right;
        input_state->cabinet_player[i].start = button_input[i].start;
    }

    m_ddrio_api.get_sys_input(&sys_input);

    input_state->cabinet_operator.test = sys_input.test;
    input_state->cabinet_operator.service = sys_input.service;
    input_state->cabinet_operator.coin1 = sys_input.coin1;
    input_state->cabinet_operator.coin2 = sys_input.coin2;
}

void ddrio_driver::_merge_input_states(
    struct input_state* merged,
    struct input_state* input_states,
    uint32_t count)
{
    for (uint32_t i = 0; i < count; i++) {
        for (uint8_t j = 0; j < DDRIO_PLAYER_NUM; j++) {
            merged->pad[j].left = merged->pad[j].left || input_states[i].pad[j].left;
            merged->pad[j].down = merged->pad[j].down || input_states[i].pad[j].down;
            merged->pad[j].up = merged->pad[j].up || input_states[i].pad[j].up;
            merged->pad[j].right = merged->pad[j].right || input_states[i].pad[j].right;
        }

        for (uint8_t j = 0; j < DDRIO_PLAYER_NUM; j++) {
            merged->cabinet_player[j].left =
                merged->cabinet_player[j].left || input_states[i].cabinet_player[j].left;
            merged->cabinet_player[j].right =
                merged->cabinet_player[j].right || input_states[i].cabinet_player[j].right;
            merged->cabinet_player[j].start =
                merged->cabinet_player[j].start || input_states[i].cabinet_player[j].start;
        }

        merged->cabinet_operator.test =
            merged->cabinet_operator.test || input_states[i].cabinet_operator.test;
        merged->cabinet_operator.service =
            merged->cabinet_operator.service || input_states[i].cabinet_operator.service;
        merged->cabinet_operator.coin1 =
            merged->cabinet_operator.coin1 || input_states[i].cabinet_operator.coin1;
        merged->cabinet_operator.coin2 =
            merged->cabinet_operator.coin2 || input_states[i].cabinet_operator.coin2;
    }
}

void ddrio_driver::_rate_limit_and_sleep(std::size_t poll_elapsed_time_us)
{
    if (m_poll_rate_hz != DDRIO_DRIVER_POLL_RATE_NO_LIMIT) {
        std::size_t poll_time_frame_us = (1000 * 1000) / m_poll_rate_hz;

        if (poll_elapsed_time_us > poll_time_frame_us) {
            printf("WARNING: Poll elapsed time (%ld us) > total poll frame time (%ld us) for %d hz poll rate\n",
                poll_elapsed_time_us,
                poll_time_frame_us,
                m_poll_rate_hz);
        } else {
            std::this_thread::sleep_for(
                std::chrono::microseconds(poll_time_frame_us - poll_elapsed_time_us));
        }
    }
}