cmake_minimum_required(VERSION 3.10)

project(dtjlp)

# This is a literate programming project, so the source files need to be tangled
# together before it can be built. To do that, there is a pre-tangled version of
# this project in build_tools/dtjlp that can be used to bootstrap the program.
# However, cmake does not allow you to have 2 of the same executable target,
# which will happen when a project has a subdirectory of itself. So when the
# DTJLP_BOOTSTRAP variable is set, the executable target is actually going to be
# dtjlp_bootstrap instead of dtjlp.
set(DTJLP_BOOTSTRAP 1)
add_subdirectory(build_tools/dtjlp)
unset(DTJLP_BOOTSTRAP)

# Tangle dtjlp.c from dtjlp.md using the dtjlp_bootstrap executable target.
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
