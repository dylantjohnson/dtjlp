cmake_minimum_required(VERSION 3.26)

project(dtjlp_build_tools)

set(DTJLP_BOOTSTRAP 1)
add_subdirectory(dtjlp)
unset(DTJLP_BOOTSTRAP)
