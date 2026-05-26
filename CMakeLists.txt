cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zmk_trackball)

target_sources(app PRIVATE src/pmw3389.c)
