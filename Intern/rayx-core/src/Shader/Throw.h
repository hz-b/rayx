#pragma once

#include "Core.h"

// throws an error, and termiantes the program
// TODO(Sven): rethink error handling. just instantly terminate with RAYX_EXIT or use recordFinalEvent?
// #define _throw(string) recordFinalEvent(_ray, EventType::FatalError)
// #define _throw(string) RAYX_ERR << string
#define _throw(string)                                             \
    printf("Error occurred while executing shader: %s\n", string); \
    assert(false)
// #endif
