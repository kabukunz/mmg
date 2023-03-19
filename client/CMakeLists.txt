#
# CMAKE VERSION
#

cmake_minimum_required(VERSION 3.16)

#
# LOGGING
#

set(CMAKE_VERBOSE_MAKEFILE ON)

#
# PROJECT
#

project(mmg2d_client)

#
# MMG
#

# set(MMG_DIR "${CMAKE_BINARY_DIR}/prebuilt/mmg" CACHE STRING "MMG DIR" FORCE)
# if(NOT MMG_DIR)
#     message(FATAL_ERROR "MMG_DIR NOT SET. EXITING")
# else()
#     message(STATUS "MMG_DIR: " "${MMG_DIR}")
# endif()

# set(MMG_SRC "/Users/max/Developer/Stage/Workspace/3D/mmg_int/mmg/")
# set(MMG_INST "${MMG_SRC}/build/Release/install/")

message(STATUS "MMG_SRC: " ${MMG_SRC})
message(STATUS "MMG_INST: " ${MMG_INST})

set(MMG_DIR "${MMG_INST}")

list(APPEND CMAKE_MODULE_PATH "${MMG_SRC}/cmake/tools")

set(MMG_INCDIR "${MMG_DIR}/include")
set(MMG_LIBDIR "${MMG_DIR}/lib")

find_package(MMG2D REQUIRED)
    # HINTS "${MMG_DIR}/lib/cmake/mmg")

message(STATUS "MMG2D_INCLUDE_DIRS: " ${MMG2D_INCLUDE_DIRS})
message(STATUS "MMG2D_LIBRARIES: " ${MMG2D_LIBRARIES})

add_library(MMG2D_DLL SHARED IMPORTED)
set_property(TARGET MMG2D_DLL PROPERTY IMPORTED_LOCATION "${MMG_DIR}/lib/mmg2d.dll")

# message(STATUS "FOO LIBS: " $<TARGET_LINKER_FILE_NAME:MMG2D_DLL>)

#
# EXE
#

add_executable(mmg2dclient "${MMG_SRC}/libexamples/mmg2d/adaptation_example0/example0_a/main.c")

# Add dependencies
target_include_directories(mmg2dclient PUBLIC ${MMG2D_INCLUDE_DIRS})

# link library
target_link_libraries(mmg2dclient PUBLIC ${MMG2D_LIBRARIES})

# Set Properties
set_target_properties(mmg2dclient PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# Copy dll (target name is unknown)
if(WIN32)
    add_custom_command(TARGET mmg2dclient POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:MMG2D_DLL> $<TARGET_FILE_DIR:mmg2dclient>
    )
endif()