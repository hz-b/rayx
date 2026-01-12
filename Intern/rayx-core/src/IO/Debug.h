#pragma once

// TODO: implement logging system for debug prints

// An empty implementation used in release when using "debug-only" prints like RAYX_D_LOG.
struct RAYX_API IgnoreLog{
    template <typename T> IgnoreLog & operator<<(T){return *this;}
};

#define RAYX_LOG  IgnoreLog()
#define RAYX_WARN IgnoreLog()
#define RAYX_EXIT IgnoreLog()
#define RAYX_VERB IgnoreLog()
