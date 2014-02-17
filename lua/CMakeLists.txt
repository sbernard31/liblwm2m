cmake_minimum_required (VERSION 2.8.3)

SET (CMAKE_BUILD_TYPE Debug)

project (lwm2mluaclient)

add_definitions(-DLWM2M_CLIENT_MODE)

include_directories (${PROJECT_SOURCE_DIR}/.. ${PROJECT_SOURCE_DIR}/../utils)

add_subdirectory(${PROJECT_SOURCE_DIR}/../core ${CMAKE_CURRENT_BINARY_DIR}/core)

SET(SOURCES lua_lwm2mclient.c lua_object.c)

add_library(lwm2m MODULE lua_object.c lua_liblwm2m.c ${CORE_SOURCES} ${EXT_SOURCES})