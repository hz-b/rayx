#pragma once

#include "Common.h"

// throws an error, and termiantes the program
// TODO(Sven): rethink error handling. just instantly terminate with RAYX_ERR or use recordFinalEvent?
// #define _throw(string) recordFinalEvent(_ray, ETYPE_FATAL_ERROR)
#include "Debug/Debug.h"
// #define _throw(string) RAYX_ERR << string
#define _throw(string) assert(false)
// #endif
