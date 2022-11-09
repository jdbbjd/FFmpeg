cmake_minimum_required (VERSION 3.0)
cmake_policy(VERSION 3.0)

project(FFmpeg)

set(CMAKE_CXX_STANDARD 17) # this does nothing for MSVC, use target_compile_options below
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS ON)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

#link_directories(${CMAKE_SOURCE_DIR}/lib)
#link_directories(${CMAKE_SOURCE_DIR}/../../../lib)
link_directories(/usr/local/lib)

# find the required packages
#find_package(GLM REQUIRED)
#message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")
#find_package(GLFW3 REQUIRED)
#message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
#find_package(ASSIMP REQUIRED)
#message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")
# find_package(SOIL REQUIRED)
# message(STATUS "Found SOIL in ${SOIL_INCLUDE_DIR}")
# find_package(GLEW REQUIRED)
# message(STATUS "Found GLEW in ${GLEW_INCLUDE_DIR}")
#if(APPLE)
#  find_package(Freetype REQUIRED)
#  message(STATUS "Found Freetype in ${FREETYPE_INCLUDE_DIRS}")
#endif(APPLE)

set(FFMPEG_SHARED_LIBRARYS
	libavdevice
	libavformat
	libavfilter
	libavcodec
	libswresample
	libswscale
	libavutil
)

if(APPLE)
  INCLUDE_DIRECTORIES(/System/Library/Frameworks)
  set(APPLE_LIBRARYS
	Security
	AVFoundation
    	CoreGraphics
	CoreFoundation
	VideoToolbox
	AudioToolbox
	CoreMedia
	CoreVideo
    	AppKit
    	CoreAudio
   	 CoreImage
    	Foundation
    	CoreHaptics
    	Metal
    	OpenGL
	libbz2.1.0.5.tbd
	libiconv.2.4.0.tbd
	liblzma.tbd
	libz.1.tbd
)

foreach(APPLE_LIBRARY ${APPLE_LIBRARYS})
  FIND_LIBRARY(${APPLE_LIBRARY}_LIBRARY ${APPLE_LIBRARY})
  #SET(APPLE_LIBS ${APPLE_LIBS} ${${APPLE_LIBRARY}_LIBRARY})
endforeach(CHAPTER)

foreach(LIBRARY ${FFMPEG_SHARED_LIBRARYS})
	FIND_LIBRARY(FFMPEG_${LIBRARY} ${LIBRARY}.dylib)
	message(STATUS "Found ${LIBRARY} in ${FFMPEG_${LIBRARY}}")
	SET(APPLE_LIBS ${APPLE_LIBS} ${FFMPEG_${LIBRARY}})
endforeach(CHAPTER)

  
  set(LIBS ${APPLE_LIBS})
endif(APPLE)

set(DEMOS
	avio_list_dir
	avio_reading
	decode_audio
	decode_video 
	demuxing_decoding
	encode_audio
	encode_video
	extract_mvs
	filtering_video
	filtering_audio
	http_multiclient
	hw_decode
	metadata
	muxing
	remuxing
	resampling_audio
	scaling_video
	transcode_aac
	transcoding
)


#configure_file(configuration/root_directory.h.in configuration/root_directory.h)
#include_directories(${CMAKE_BINARY_DIR}/configuration)

# first create relevant static libraries requried for other projects
#add_library(STB_IMAGE "src/stb_image.cpp")
#set(LIBS ${LIBS} STB_IMAGE)

#add_library(GLAD "src/glad.c")
#set(LIBS ${LIBS} GLAD)

macro(makeLink src dest target)
  add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${src} ${dest}  DEPENDS  ${dest} COMMENT "mklink ${src} -> ${dest}")
endmacro()

function(create_project_from_sources demo)
	file(GLOB SOURCE
            "${demo}.c"
    	)
	set(NAME "${demo}")
    	add_executable(${NAME} ${SOURCE})
    	target_link_libraries(${NAME} ${LIBS})

    	if(APPLE)
		set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${demo}")
		set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/${demo}")
		set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/${demo}")
   	endif(APPLE)
endfunction()

# then create a project file per tutorial
foreach(DEMO ${DEMOS})
	create_project_from_sources(${DEMO})
endforeach(CHAPTER)

include_directories(${CMAKE_SOURCE_DIR}/../../build)
include_directories(${CMAKE_SOURCE_DIR}/../..)
