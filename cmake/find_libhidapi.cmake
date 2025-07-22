if(NOT TARGET libhidapi)
    set(LIBNAME      libhidapi)
    set(LIB_DIR_NAME hidapi)
endif()

if("$ENV{HomeDir}" STREQUAL "")
    if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
        set(ALL_LIB_HOME_DIR ${CMAKE_CURRENT_LIST_DIR}/../out/linux)
    elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
        set(ALL_LIB_HOME_DIR ${CMAKE_CURRENT_LIST_DIR}/../out/windows)
        string(REPLACE "/" "\\" ALL_LIB_HOME_DIR "${ALL_LIB_HOME_DIR}")
    else()
        message("current platform: unkonw ") 
    endif()
else()
    set(ALL_LIB_HOME_DIR "$ENV{HomeDir}")
endif()

set(LIBHIDAPI_ROOT_DIR     ${ALL_LIB_HOME_DIR}/${LIB_DIR_NAME})

set(LIBHIDAPI_INCLUDE_DIRS ${LIBHIDAPI_ROOT_DIR}/include   ${LIBHIDAPI_ROOT_DIR}/include/hidapi ${LIBHIDAPI_ROOT_DIR}/hidapi)
set(LIBHIDAPI_LIBRARY_DIRS ${LIBHIDAPI_ROOT_DIR}/lib)

set(LIBHIDAPI_LIBRARY      hidapi-hidraw  )
set(LIBHIDAPI_LIBRARIES    hidapi-hidraw  )

include_directories(${LIBHIDAPI_INCLUDE_DIRS})
link_directories   (${LIBHIDAPI_LIBRARY_DIRS})

message("[INFO] CMAKE_CURRENT_LIST_FILE    = ${CMAKE_CURRENT_LIST_FILE}") 
message("[INFO] LIBHIDAPI_ROOT_DIR         = ${LIBHIDAPI_ROOT_DIR} ") 
message("[INFO] LIBHIDAPI_INCLUDE_DIRS     = ${LIBHIDAPI_INCLUDE_DIRS} ") 
