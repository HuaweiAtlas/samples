#[==[
Provides the following variables:

  * `FFMPEG_INCLUDE_DIRS`: Include directories necessary to use FFMPEG.
  * `FFMPEG_LIBRARIES`: Libraries necessary to use FFMPEG. Note that this only
    includes libraries for the components requested.
  * `FFMPEG_VERSION`: The version of FFMPEG found.

The following components are supported:

  * `avcodec`
  * `avdevice`
  * `avfilter`
  * `avformat`
  * `avresample`
  * `avutil`
  * `swresample`
  * `swscale`

For each component, the following are provided:

  * `FFMPEG_<component>_FOUND`: Libraries for the component.
  * `FFMPEG_<component>_INCLUDE_DIRS`: Include directories for
    the component.
  * `FFMPEG_<component>_LIBRARIES`: Libraries for the component.
  * `FFMPEG::<component>`: A target to use with `target_link_libraries`.

Note that only components requested with `COMPONENTS` or `OPTIONAL_COMPONENTS`
are guaranteed to set these variables or provide targets.
#]==]

if (DEFINED FFMPEG_PATH)
  set(CMAKE_LIBRARY_PATH ${FFMPEG_PATH}/lib)
  set(FFMPEG_INCLUDE_DIRS ${FFMPEG_PATH}/include)
endif()

set(components avcodec avdevice avfilter avformat avutil swscale)

foreach(component ${components})

  if (NOT DEFINED FFMPEG_PATH)
    find_path(${component}_include_dir lib${component}/${component}.h)
    if(${component}_include_dir)
      list(APPEND FFMPEG_INCLUDE_DIRS ${${component}_include_dir})
    endif()
  endif()

  find_library(${component}_library ${component})
  if(${component}_library)
    list(APPEND FFMPEG_LIBRARIES ${${component}_library})
  endif()

endforeach()
