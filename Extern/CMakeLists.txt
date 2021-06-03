cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set( gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll" FORCE)

add_subdirectory(googletest)