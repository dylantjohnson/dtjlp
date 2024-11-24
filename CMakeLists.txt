cmake_minimum_required(VERSION 3.10)

project(dtjlp)

#add_subdirectory(source_maker)
#add_custom_command(
#	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/hello.c
#	COMMAND source_maker ${CMAKE_CURRENT_SOURCE_DIR}/hello.c
#	DEPENDS source_maker
#)
add_executable(dtjlp dtjlp.c)
