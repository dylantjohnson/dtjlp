cmake_minimum_required(VERSION 3.10)

project(dtjlp)

set(DTJLP_BOOTSTRAP 1)
add_subdirectory(build_tools/dtjlp)
unset(DTJLP_BOOTSTRAP)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/dtjlp.c
	COMMAND dtjlp_bootstrap ${CMAKE_CURRENT_SOURCE_DIR}/dtjlp.md
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dtjlp.md
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

if (DTJLP_BOOTSTRAP)
	add_executable(dtjlp_bootstrap dtjlp.c)
else()
	add_executable(dtjlp dtjlp.c)
endif()
