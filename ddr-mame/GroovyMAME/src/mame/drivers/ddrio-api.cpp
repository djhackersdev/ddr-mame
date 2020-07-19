#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ddrio-api.h"

static void* _ddrio_api_lib_load_library(const char* path)
{
	void* handle = dlopen(path, RTLD_NOW);

	if (!handle) {
        printf("ERROR: Failed to open library %s: %s\n", path, dlerror());
    } else {
		printf("Opened lib %s, handle %p\n", path, handle);
	}

	return handle;
}

static void* _ddrio_api_lib_get_func_addr_handle(void* lib_handle, const char* func_name)
{
	void* ret = dlsym(lib_handle, func_name);

    if (ret == NULL) {
        printf("ERROR: Could not find func %s in library %p\n", func_name, lib_handle);
    }

	return ret;
}

bool ddrio_api_load(const char* path, struct ddrio_api* api)
{
    void* handle;

    handle = _ddrio_api_lib_load_library(path);

    if (handle == NULL) {
        return false;
    }

    api->ident = (ddrio_ident_t) _ddrio_api_lib_get_func_addr_handle(
        handle, "ddrio_ident");
    api->open = (ddrio_open_t) _ddrio_api_lib_get_func_addr_handle(
        handle, "ddrio_open");
    api->close = (ddrio_close_t) _ddrio_api_lib_get_func_addr_handle(
        handle, "ddrio_close");
    api->update = (ddrio_update_t) _ddrio_api_lib_get_func_addr_handle(
        handle, "ddrio_update");
    api->get_pad_input = (ddrio_get_pad_input_t) _ddrio_api_lib_get_func_addr_handle(
        handle, "ddrio_get_pad_input");
    api->get_button_input = (ddrio_get_button_input_t) _ddrio_api_lib_get_func_addr_handle(
        handle, "ddrio_get_button_input");
    api->get_sys_input = (ddrio_get_sys_input_t) _ddrio_api_lib_get_func_addr_handle(
        handle, "ddrio_get_sys_input");
    api->get_pad_output = (ddrio_get_pad_output_t) _ddrio_api_lib_get_func_addr_handle(
        handle, "ddrio_get_pad_output");
    api->set_pad_output = (ddrio_set_pad_output_t) _ddrio_api_lib_get_func_addr_handle(
        handle, "ddrio_set_pad_output");
    api->get_button_output = (ddrio_get_button_output_t) _ddrio_api_lib_get_func_addr_handle(
        handle, "ddrio_get_button_output");
    api->set_button_output = (ddrio_set_button_output_t) _ddrio_api_lib_get_func_addr_handle(
        handle, "ddrio_set_button_output");
    api->get_cabinet_output = (ddrio_get_cabinet_output_t) _ddrio_api_lib_get_func_addr_handle(
        handle, "ddrio_get_cabinet_output");
    api->set_cabinet_output = (ddrio_set_cabinet_output_t) _ddrio_api_lib_get_func_addr_handle(
        handle, "ddrio_set_cabinet_output");

    api->initialized = true;

    return true;
}