cmake_minimum_required(VERSION 3.10)

project(dtjlp)

if (DTJLP_BOOTSTRAP)
	add_executable(dtjlp_bootstrap dtjlp.c)
else()
	add_executable(dtjlp dtjlp.c)
endif()
