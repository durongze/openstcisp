cmake_minimum_required(VERSION 3.16)

project(open_stc_isp)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON) 

set(ENV{HomeDir}  ${CMAKE_CURRENT_LIST_DIR}/out/${CMAKE_SYSTEM_NAME})

set (OPEN_STC_ISP_BIN_DIR  ${CMAKE_CURRENT_BINARY_DIR})
set (OPEN_STC_ISP_TOP_DIR  ${CMAKE_CURRENT_SOURCE_DIR})  #  CMAKE_SOURCE_DIR
set (OPEN_STC_ISP_SRC_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src)

set (OPEN_STC_ISP_INC_DIRS ${OPEN_STC_ISP_TOP_DIR}/     ${OPEN_STC_ISP_SRC_DIR}/       )

#  Set Target Info
set(CMAKE_INSTALL_PREFIX           ${CMAKE_BINARY_DIR}/install CACHE PATH "Where to install output files")
set(CMAKE_DEBUG_POSTFIX            d                      )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "64-bit _O_SRCS: ${_O_SRCS}")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(STATUS "32-bit _O_SRCS: ${_O_SRCS}")
else()
    message(STATUS "Unknown architecture")
endif()

configure_file     (${OPEN_STC_ISP_SRC_DIR}/stc_isp_config.h.in             ${OPEN_STC_ISP_SRC_DIR}/stc_isp_config.h)

file(GLOB         OPEN_STC_ISP_SRCS_LIST    RELATIVE ${OPEN_STC_ISP_TOP_DIR} "${OPEN_STC_ISP_SRC_DIR}/*.c*" "${OPEN_STC_ISP_SRC_DIR}/*.h*")
file(GLOB         OPEN_STC_ISP_APP_LIST     RELATIVE ${OPEN_STC_ISP_TOP_DIR} "${OPEN_STC_ISP_SRC_DIR}/main.c*"  )
file(GLOB         OPEN_STC_ISP_SRCS_FILTER  RELATIVE ${OPEN_STC_ISP_TOP_DIR} "${OPEN_STC_ISP_SRC_DIR}/BusHoundLog.c*"  )

file(GLOB         OPEN_STC_ISP_HDRS_LIST    RELATIVE ${OPEN_STC_ISP_TOP_DIR} ${OPEN_STC_ISP_SRC_DIR}/*.h)

source_group(TREE ${OPEN_STC_ISP_TOP_DIR}    FILES ${OPEN_STC_ISP_SRCS_LIST} )

list(REMOVE_ITEM  OPEN_STC_ISP_SRCS_LIST ${OPEN_STC_ISP_APP_LIST} ${OPEN_STC_ISP_SRCS_FILTER})

message("[INFO] OPEN_STC_ISP_SRCS_LIST:${OPEN_STC_ISP_SRCS_LIST}")

include(cmake/find_libhidapi.cmake)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set (sys_dep_libs            setupapi winmm imm32 version                                )
endif()

set (OPEN_STC_ISP_DEP_LIBS   ${LIBHIDAPI_LIBRARIES}      )

#set (OPEN_STC_ISP_DEP_DEFS    )

set                       (  OPEN_STC_ISP_STATIC               open_stc_isp_static)
add_library               (${OPEN_STC_ISP_STATIC}   STATIC     ${OPEN_STC_ISP_SRCS_LIST})
target_compile_definitions(${OPEN_STC_ISP_STATIC}   PRIVATE    ${OPEN_STC_ISP_DEP_DEFS}      OPEN_STC_ISP_LIB_STATIC)
target_include_directories(${OPEN_STC_ISP_STATIC}   PUBLIC     ${OPEN_STC_ISP_INC_DIRS} )
target_link_directories   (${OPEN_STC_ISP_STATIC}   PUBLIC     ${OPEN_STC_ISP_LIB_DIRS} )
target_link_libraries     (${OPEN_STC_ISP_STATIC}   PUBLIC     ${OPEN_STC_ISP_DEP_LIBS}    ${sys_dep_libs}             )
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    target_compile_options    (${OPEN_STC_ISP_STATIC}   PRIVATE     /Z7     /W4          )
endif()
set_target_properties     (${OPEN_STC_ISP_STATIC}   PROPERTIES CLEAN_DIRECT_OUTPUT 1     OUTPUT_NAME "open_stc_isp"   )

set                       (  OPEN_STC_ISP_SHARED               open_stc_isp_shared)
add_library               (${OPEN_STC_ISP_SHARED}   SHARED     ${OPEN_STC_ISP_SRCS_LIST})
target_compile_definitions(${OPEN_STC_ISP_SHARED}   PRIVATE    ${OPEN_STC_ISP_DEP_DEFS}     OPEN_STC_ISP_LIB_EXPORTS)
target_include_directories(${OPEN_STC_ISP_SHARED}   PUBLIC     ${OPEN_STC_ISP_INC_DIRS} )
target_link_directories   (${OPEN_STC_ISP_SHARED}   PUBLIC     ${OPEN_STC_ISP_LIB_DIRS} )
target_link_libraries     (${OPEN_STC_ISP_SHARED}   PUBLIC     ${OPEN_STC_ISP_DEP_LIBS}    ${sys_dep_libs}             ) #  ${sys_dep_libs}
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    target_compile_options    (${OPEN_STC_ISP_SHARED}   PRIVATE     /Z7     /W4          )
endif()
set_target_properties     (${OPEN_STC_ISP_SHARED}   PROPERTIES CLEAN_DIRECT_OUTPUT 1     OUTPUT_NAME "open_stc_isp"   )
set_target_properties     (${OPEN_STC_ISP_SHARED}   PROPERTIES PUBLIC_HEADER             "${OPEN_STC_ISP_HDRS_LIST}")

set                       (  OPEN_STC_ISP_APP                open_stc_isp_app)
add_executable            (${OPEN_STC_ISP_APP}               ${OPEN_STC_ISP_APP_LIST} )
target_compile_definitions(${OPEN_STC_ISP_APP}   PRIVATE     ${OPEN_STC_ISP_DEP_DEFS} )
target_include_directories(${OPEN_STC_ISP_APP}   PUBLIC      ${OPEN_STC_ISP_INC_DIRS} )
target_link_directories   (${OPEN_STC_ISP_APP}   PUBLIC      ${OPEN_STC_ISP_LIB_DIRS} )
#target_link_libraries     (${OPEN_STC_ISP_APP}               ${OPEN_STC_ISP_STATIC}   )  # 
target_link_libraries     (${OPEN_STC_ISP_APP}               ${OPEN_STC_ISP_SHARED}   )  # must be REALSE

SET(install_targets ${OPEN_STC_ISP_STATIC}    ${OPEN_STC_ISP_SHARED}    ${OPEN_STC_ISP_APP})
SET(install_hdr_dir include)

INSTALL(TARGETS ${install_targets}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION ${install_hdr_dir}
       )

install(FILES      ${OPEN_STC_ISP_HDRS_LIST} DESTINATION ${install_hdr_dir})
install(DIRECTORY  "inc_sub_dir"             DESTINATION ${install_hdr_dir} FILES_MATCHING PATTERN "*.h")
