#[=======================================================================[.rst:
FindDDK
---------
You Must Export DDK_HOME At First!!!
it may be /home/youruser/tools/che/ddk/ddk

DDK headers and libraries

    DDK_INCLUDE_DIRS        - 
    DDK_HOST_LIBRARIES      -
    DDK_DEVICE_LIBRARIES    -
    DDK_FOUND               -

DDK Third_party

    cereal
        DDK_CEREAL_INCLUDE_DIRS     - 

    protobuf
        DDK_PROTOBUF_INCLUDE_DIRS   -  
        DDK_PROTOBUF_LIBRARYS       -

    glog
        DDK_GLOG_INCLUDE_DIRS       - 
        DDK_GLOG_LIBRARYS           -

    gflags
        DDK_GFLAGS_INCLUDE_DIRS     -
        DDK_GFLAGS_LIBRARYS         -

    opencv
        DDK_OPENCV_INCLUDE_DIRS     -
        DDK_OPENCV_LIBRARYS         -
    
    openssl
        DDK_OPENSSL_LIBRARYS        -

#]=======================================================================]

if(NOT DEFINED ENV{DDK_HOME})
    message(FATAL_ERROR "please define environment variable:DDK_HOME")  
endif()

set(_DDK_ROOT_PATHS PATHS $ENV{DDK_HOME} NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_PACKAGE_REGISTRY NO_CMAKE_FIND_ROOT_PATH)

#
set(_DDK_HOST_LIB_PATHS ${_DDK_ROOT_PATHS})
if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL aarch64)
    set(_DDK_HOST_PATH_SUFFIXES "lib64")
else()
    set(_DDK_HOST_PATH_SUFFIXES "uihost/lib")
endif()

#
set(_DDK_DEVICE_PATH_SUFFIXES "device/lib")

#
function(find_ddk_host_lib _var _names)
    find_library(${_var} NAMES ${_names} NAMES_PER_DIR ${_DDK_ROOT_PATHS} PATH_SUFFIXES ${_DDK_HOST_PATH_SUFFIXES} NO_CMAKE_SYSTEM_PATH)
endfunction()

function(find_ddk_device_lib _var _names)
    find_library(${_var} NAMES ${_names} NAMES_PER_DIR ${_DDK_ROOT_PATHS} PATH_SUFFIXES ${_DDK_DEVICE_PATH_SUFFIXES} NO_CMAKE_SYSTEM_PATH)
endfunction()

if(NOT DDK_FOUND)

    #ddk include path
    set(DDK_INCLUDE_DIRS ${DDK_INCLUDE_DIRS} $ENV{DDK_HOME}/include)
    set(DDK_INCLUDE_DIRS ${DDK_INCLUDE_DIRS} $ENV{DDK_HOME}/include/inc)
    set(DDK_INCLUDE_DIRS ${DDK_INCLUDE_DIRS} $ENV{DDK_HOME}/include/libc_sec/include)

    #ddk lib
    #c_sec
    find_ddk_host_lib(DDK_CSEC_LIBRARY c_sec)

    #common
    find_ddk_host_lib(DDK_DRVDEVDRV_LIBRARY drvdevdrv)
    find_ddk_host_lib(DDK_DRVHDC_HOST_LIBRARY drvhdc_host)
    find_ddk_host_lib(DDK_MMPA_LIBRARY mmpa)
    find_ddk_host_lib(DDK_MEMORY_LIBRARY memory)
    find_ddk_host_lib(DDK_MATRIX_LIBRARY matrix)
    find_ddk_host_lib(DDK_PROFILERCLIENT_LIBRARY profilerclient)
    find_ddk_host_lib(DDK_SLOG_LIBRARY slog)

    set(DDK_HOST_LIBRARIES 
        ${DDK_DRVDEVDRV_LIBRARY} 
        ${DDK_DRVHDC_HOST_LIBRARY}
        ${DDK_MMPA_LIBRARY} 
		${DDK_MEMORY_LIBRARY}
        ${DDK_MATRIX_LIBRARY} 
        ${DDK_PROFILERCLIENT_LIBRARY} 
        ${DDK_SLOG_LIBRARY} 
        ${DDK_CSEC_LIBRARY})


    #device
    find_ddk_device_lib(DDK_IDEDAEMON_LIBRARY idedaemon)


    #dvpp lib
    find_ddk_device_lib(DDK_DVPP_API_LIBRARY Dvpp_api)
    find_ddk_device_lib(DDK_DVPP_JPEG_D_LIBRARY Dvpp_jpeg_decoder)
    find_ddk_device_lib(DDK_DVPP_JEPG_E_LIBRARY Dvpp_jpeg_encoder)
    find_ddk_device_lib(DDK_DVPP_PNG_D_LIBRARY Dvpp_png_decoder)
    find_ddk_device_lib(DDK_DVPP_VPC_LIBRARY Dvpp_vpc)
    set(DDK_DVPP_LIBRARYS 
        ${DDK_DVPP_API_LIBRARY} 
        ${DDK_DVPP_JPEG_D_LIBRARY} 
        ${DDK_DVPP_JEPG_E_LIBRARY} 
        ${DDK_DVPP_PNG_D_LIBRARY} 
        ${DDK_DVPP_VPC_LIBRARY})

    set(DDK_DEVICE_LIBRARIES ${DDK_IDEDAEMON_LIBRARY} ${DDK_DVPP_LIBRARYS})

    #third_party

    set(_DDK_THIRD_INC_DIR "include/third_party")
    set(_DDK_THIRD_INC_SUFFIXES 
        ${_DDK_THIRD_INC_DIR}/cereal/include 
        ${_DDK_THIRD_INC_DIR}/gflags/include 
        ${_DDK_THIRD_INC_DIR}/glog/include 
        ${_DDK_THIRD_INC_DIR}/protobuf/include
        ${_DDK_THIRD_INC_DIR}/opencv/include)

    #cereal - only header file(hpp)
    find_path(DDK_CEREAL_INCLUDE_DIRS cereal/cereal.hpp PATHS ${_DDK_ROOT_PATHS} PATH_SUFFIXES ${_DDK_THIRD_INC_SUFFIXES})

    #glog
    find_path(DDK_GLOG_INCLUDE_DIR glog/logging.h PATHS ${_DDK_ROOT_PATHS} PATH_SUFFIXES ${_DDK_THIRD_INC_SUFFIXES})
    find_ddk_host_lib(DDK_GLOG_LIBRARYS glog)

    #gflags
    find_path(DDK_GFLAGS_INCLUDE_DIRS gflags/gflags.h PATHS ${_DDK_ROOT_PATHS} PATH_SUFFIXES ${_DDK_THIRD_INC_SUFFIXES})
    find_ddk_host_lib(DDK_GFLAGS_LIBRARYS gflags)

    #openssl
    find_ddk_host_lib(DDK_OPENSSL_SSL_LIBRARY ssl)
    find_ddk_host_lib(DDK_OPENSSL_CRYPTO_LIBRARY crypto)
    set(DDK_OPENSSL_LIBRARYS ${DDK_OPENSSL_SSL_LIBRARY} ${DDK_OPENSSL_CRYPTO_LIBRARY})

    #protobuf
    find_path(DDK_PROTOBUF_INCLUDE_DIRS google/protobuf/message.h PATHS ${_DDK_ROOT_PATHS} PATH_SUFFIXES ${_DDK_THIRD_INC_SUFFIXES})
    find_ddk_host_lib(DDK_PROTOBUF_LIBRARYS protobuf)

    #opencv
    find_path(DDK_OPENCV_INCLUDE_DIRS opencv2/opencv.hpp PATHS ${_DDK_ROOT_PATHS} PATH_SUFFIXES ${_DDK_THIRD_INC_SUFFIXES})
    find_ddk_host_lib(DDK_OPENCV_LIBRARYS opencv_world)

    #
    set(DDK_THIRD_LIBRARYS 
        ${DDK_GLOG_LIBRARYS} 
        ${DDK_GFLAGS_LIBRARYS}
        ${DDK_PROTOBUF_LIBRARYS} 
        ${DDK_OPENCV_LIBRARYS})

    #
    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL aarch64)
    	set(DDK_HOST_LIBRARIES ${DDK_HOST_LIBRARIES} ${DDK_PROTOBUF_LIBRARYS} ${DDK_OPENSSL_LIBRARYS})  
    endif()

    #
    set(DDK_FOUND TRUE)
endif(NOT DDK_FOUND)
