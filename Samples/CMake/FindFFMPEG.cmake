#[==[
#Provides the following variables:

#  * `FFMPEG_INCLUDE_DIRS`: Include directories necessary to use FFMPEG.
#  * `FFMPEG_LIBRARIES`: Libraries necessary to use FFMPEG. Note that this only
#    includes libraries for the components requested.
#  * `FFMPEG_VERSION`: The version of FFMPEG found.

#The following components are supported:

#  * `avcodec`
#  * `avdevice`
#  * `avfilter`
#  * `avformat`
#  * `avresample`
#  * `avutil`
#  * `swresample`
#  * `swscale`

#For each component, the following are provided:

#  * `FFMPEG_<component>_FOUND`: Libraries for the component.
#  * `FFMPEG_<component>_INCLUDE_DIRS`: Include directories for
#    the component.
#  * `FFMPEG_<component>_LIBRARIES`: Libraries for the component.
#  * `FFMPEG::<component>`: A target to use with `target_link_libraries`.

#Note that only components requested with `COMPONENTS` or `OPTIONAL_COMPONENTS`
#are guaranteed to set these variables or provide targets.
#]==]

if (NOT DEFINED FFMPEG_PATH)
  message(FATAL_ERROR "please define environment variable:FFMPEG_PATH")
endif()

set(FFMPEG_INCLUDE_DIRS ${FFMPEG_PATH}/include)

set(components avcodec avdevice avfilter avformat avutil swscale)

foreach(component ${components})

  if (NOT DEFINED FFMPEG_PATH)
    find_path(${component}_include_dir lib${component}/${component}.h)
    if(${component}_include_dir)
      list(APPEND FFMPEG_INCLUDE_DIRS ${${component}_include_dir})
    endif()
  endif()

  set(FFMPEG_LIB_PATH PATHS ${FFMPEG_PATH} NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_PACKAGE_REGISTRY NO_CMAKE_FIND_ROOT_PATH)
  set(FFMPEG_LIB_SUFFIXES "lib")
  find_library(${component}_library NAMES ${component} NAMES_PER_DIR ${FFMPEG_LIB_PATH} PATH_SUFFIXES ${FFMPEG_LIB_SUFFIXES} NO_CMAKE_SYSTEM_PATH)
  if(${component}_library)
    list(APPEND FFMPEG_LIBRARIES ${${component}_library})
  endif()

endforeach()
