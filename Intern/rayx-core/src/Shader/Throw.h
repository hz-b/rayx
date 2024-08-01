#pragma once

#include "Core.h"

// throws an error, and termiantes the program
// TODO(Sven): rethink error handling. just instantly terminate with RAYX_EXIT or use recordFinalEvent?
// #define _throw(string) recordFinalEvent(_ray, ETYPE_FATAL_ERROR)
#include "Debug/Debug.h"
// #define _throw(string) RAYX_EXIT << string
#define _throw(string) assert(false)
// #endif
