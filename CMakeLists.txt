cmake_minimum_required(VERSION 3.12)
project(SimplePlayer C)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES SimplePlayer.c)


# 这里我使用的是自己机器上安装的FFmpeg、SDL库，根据自己的安装路径替换
set(INC_DIR C:/cygwin64/usr/local/ffmpeg/include/)
set(LINK_DIR C:/cygwin64/usr/local/ffmpeg/lib/)

set(INC_DIR_SDL C:/cygwin64/usr/local/include/SDL2/)
set(LINK_DIR_SDL C:/cygwin64/usr/local/lib/)

include_directories(${INC_DIR_SDL})
link_directories(${LINK_DIR_SDL})
include_directories(${INC_DIR})
link_directories(${LINK_DIR})

add_executable(SimplePlayer ${SOURCE_FILES})
target_link_libraries(
        SimplePlayer
        SDL2
        SDL2main
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        postproc
        swresample
        pthread
        swscale)