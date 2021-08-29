cmake_minimum_required (VERSION 3.16)
project (libhttpwebserver_socket
         VERSION 2.1.0
         DESCRIPTION "TCP/IP Socket Library")

include (CheckIncludeFiles)
include (CheckIncludeFileCxx)

set (CMAKE_CXX_STANDARD 14)

string (REPLACE "." "," PRODUCT_FILE_VERSION ${PROJECT_VERSION})

#---------------------------------------------------------------------

option (DEBUG "Compile with debug symbols." OFF)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set (DEBUG ON)
endif ()

if (NOT DEBUG)
    set (HTTP_WEB_SERVER_DEBUG OFF)
    set (RELEASE ON)
    set (DEBUG OFF)
    set (_DEBUG OFF)
    set (NDEBUG ON)
else ()
    set (HTTP_WEB_SERVER_DEBUG ON)
    set (RELEASE OFF)
    set (DEBUG ON)
    set (_DEBUG ON)
    set (NDEBUG OFF)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set (DEBUGINFO ON)
endif ()

option (VERBOSE "Verbose console output." OFF)
if (VERBOSE)
    set (CMAKE_VERBOSE_MAKEFILE ON)
endif ()

option (HTTPWEBSERVER_EXPORT_LIB "Build Shared Libraries." ON)
set (HTTPWEBSERVER_LIB_EXPORT_SHARED OFF)
set (HTTPWEBSERVER_LIB_EXPORT_STATIC OFF)
if (HTTPWEBSERVER_EXPORT_LIB)
    set (HTTPWEBSERVER_LIB_TYPE SHARED)
    set (HTTPWEBSERVER_LIB_EXPORT_SHARED ON)
else ()
    set (HTTPWEBSERVER_LIB_TYPE STATIC)
    set (HTTPWEBSERVER_LIB_EXPORT_STATIC ON)
endif ()

#---------------------------------------------------------------------

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set (LINUX ON)
endif ()

if (WIN32)
    # operating system for the spoiled and the losers
    set (LOSER ON)
endif ()
# no support for the weirdos using APPLE

if (LOSER)
    check_include_file_cxx ("winsock2.h" HAVE_WINSOCK2)
    message (STATUS "Searching for winsock2")
    if (NOT HAVE_WINSOCK2)
        message (FATAL_ERROR "cannot find winsock2.h")
    endif ()
endif ()

#check_include_file ("fcntl.h" HAVE_FCNTL)
#message (STATUS "Searching for fcntl")
#if (NOT HAVE_FCNTL)
#    message (FATAL_ERROR "cannot find fcntl.h")
#endif ()

if (LOSER)
    if (MSVC)
        set (DLLTOOL OFF)
    else ()
        message (STATUS "Detecting dlltool")
        find_program (DLLTOOL dlltool)
        if (DLLTOOL)
            message (STATUS "  Found dlltool: ${DLLTOOL}")
        else ()
            message (WARNING "dlltool not found. Skipping import library generation.")
        endif ()
    endif ()

    if (MINGW)
        set (CMAKE_RC_COMPILER_INIT windres)
        enable_language (RC)
        set (CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <DEFINES> -o <OBJECT> -i <SOURCE>")
    endif ()

endif ()

#---------------------------------------------------------------------

set (SOURCE_FILES "socket.cpp")
if (LOSER)
    list (APPEND SOURCE_FILES "manifest.rc")
endif ()

if (VERBOSE)
    message(STATUS "${CMAKE_PROJECT_NAME} files:")
    foreach (_variableName ${SOURCE_FILES})
        message(STATUS "  - ${_variableName}")
    endforeach()
endif ()

set (BINARY_NAME "httpwebserver_socket")
add_library (${BINARY_NAME} ${HTTPWEBSERVER_LIB_TYPE} ${SOURCE_FILES})

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/socket.hpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/socket.hpp")
if (LOSER)
    configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/manifest.rc.in" "${CMAKE_CURRENT_SOURCE_DIR}/manifest.rc")
endif ()

#---------------------------------------------------------------------

set (COMMON_COMPILE_OPTIONS "")
set (COMMON_DEBUG_COMPILE_OPTTIONS "")

if (NOT MSVC)
    list (APPEND COMMON_COMPILE_OPTIONS "-Wno-missing-braces"
                                        "-Wall"
                                        "-Wextra"
                                        #-Wfatal-errors
                                        "-fno-strict-aliasing")
    #add_definitions ("--pedantic"
    #                 "-ansi")

    if (DEBUG)
        list (APPEND COMMON_DEBUG_COMPILE_OPTTIONS #"-Wno-bool-compare"
                                                   #"-Wno-incompatible-pointer-types"
                                                   "-Wno-unused-parameter"
                                                   #"-Wno-unused-variable"
                                                   "-Wno-unused-function"
                                                   "-Wno-format")
    endif ()

endif ()

if (DEBUG OR DEBUGINFO)
    message (STATUS "Compiling with DEBUG symbols")

    if (NOT MSVC)
        add_definitions (-g3
                         #-gz=zlib
                         -fdiagnostics-show-option
                         -femit-class-debug-always
                         -fvar-tracking
                         -fmax-errors=3)
    else ()
        list (APPEND COMMON_COMPILE_OPTIONS "-Zi")
    endif ()
endif ()

if (NOT DEBUG)
    message (STATUS "Compiling with optimizations")

    if (NOT MSVC)
        list (APPEND COMMON_COMPILE_OPTIONS "-O3"
                                            "-s"
                                            "-D_FORTIFY_SOURCE=2")
    else ()
        list (APPEND COMMON_COMPILE_OPTIONS #"-Ox"
                                            "-GF"
                                            "-GL"
                                            "-LTCG"
                                             )
    endif ()
endif ()

#---------------------------------------------------------------------

if (${HTTPWEBSERVER_LIB_TYPE} STREQUAL "STATIC")
    list (APPEND COMMON_COMPILE_OPTIONS "-fPIC"
          )
    add_definitions (-DHTTPWEBSERVER_SOCKET_LIB_EXPORT=1
                     )
elseif (${HTTPWEBSERVER_LIB_TYPE} STREQUAL "SHARED")
    if (LOSER)
        set (CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

        if (MSVC)
            set_target_properties (${BINARY_NAME} PROPERTIES
                                                  LINK_FLAGS "${CMAKE_SHARED_LINKER_FLAGS_INIT} \/DYNAMICBASE")
        endif ()

        if (DLLTOOL)
            set_target_properties (${BINARY_NAME} PROPERTIES OUTPUT_NAME "${BINARY_NAME}"
                                                  ARCHIVE_OUTPUT_NAME "${BINARY_NAME}"
                                                  LINK_FLAGS "${CMAKE_SHARED_LINKER_FLAGS_INIT} -Wl,--output-def=lib${BINARY_NAME}.def")

            add_custom_command(TARGET ${BINARY_NAME} POST_BUILD
                               #WORKING_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
                               COMMAND echo "Generating import library"
                               COMMAND "${DLLTOOL}" --dllname "lib${BINARY_NAME}.dll"
                                                    --input-def "lib${BINARY_NAME}.def"
                                                    --output-lib "lib${BINARY_NAME}.lib"
                                                    #todo add --as-flags=--64
                                                    )

        endif ()
    else ()
        set_target_properties (${BINARY_NAME} PROPERTIES OUTPUT_NAME "httpwebserver_socket"
                                                         ARCHIVE_OUTPUT_NAME "httpwebserver_socket")
    endif ()

    #if (LINUX)
    #    set_target_properties (${BINARY_NAME} PROPERTIES OUTPUT_NAME "${BINARY_NAME}"
    #                           ARCHIVE_OUTPUT_NAME "${BINARY_NAME}"
    #                           LINK_FLAGS ${CMAKE_SHARED_LINKER_FLAGS_INIT}
    #                                      "-Wl,-rpath=./")
    #
    #endif ()
    list (APPEND COMMON_COMPILE_OPTIONS -DHTTPWEBSERVER_SOCKET_LIB_EXPORT=1
          )

    set_target_properties(${BINARY_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
    set_target_properties(${BINARY_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
endif ()

target_compile_options (${BINARY_NAME} PRIVATE ${COMMON_COMPILE_OPTIONS}
                        ${COMMON_DEBUG_COMPILE_OPTTIONS})
set_property (TARGET ${BINARY_NAME} PROPERTY CXX_STANDARD 14)

set (COMMON_LINK_LIBRARIES "")
if (LOSER)
    list (APPEND COMMON_LINK_LIBRARIES "wsock32")
endif ()
target_link_libraries (${BINARY_NAME} ${COMMON_LINK_LIBRARIES})

target_include_directories(${BINARY_NAME}
                           PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                                  "$<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")

#---------------------------------------------------------------------

add_executable ("hws_socket" "main.cxx")
target_include_directories ("hws_socket" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries ("hws_socket" ${BINARY_NAME}
                                      )
