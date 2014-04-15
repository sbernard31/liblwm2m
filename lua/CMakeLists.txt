cmake_minimum_required (VERSION 2.8.3)

SET (CMAKE_BUILD_TYPE Debug)

project (lwm2mluaclient)

SET(LIBLWM2M_DIR ${PROJECT_SOURCE_DIR}/../core)

add_definitions(-DLWM2M_CLIENT_MODE)

include_directories (${LIBLWM2M_DIR} ${PROJECT_SOURCE_DIR}/../utils)

add_subdirectory(${LIBLWM2M_DIR} ${CMAKE_CURRENT_BINARY_DIR}/core)

SET(SOURCES lua_liblwm2m.c lua_object.c)


add_library(lwm2m MODULE ${SOURCES} ${CORE_SOURCES})
SET_TARGET_PROPERTIES(lwm2m PROPERTIES PREFIX "")