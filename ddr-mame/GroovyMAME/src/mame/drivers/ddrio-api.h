#pragma once

#include "ddrio.h"

/**
 * Load a dynamic library implementing the DDRIO API.
 * 
 * @param path Path to the dynamic library, e.g. .so/.dll file
 * @param api Pointer to a struct to populate to access the loaded API implementation.
 * @return True on success, false on error, e.g. file not found, loading failed, resolving
 *          functions failed.
 */
bool ddrio_api_load(const char* path, struct ddrio_api* api);