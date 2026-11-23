cmake_minimum_required(VERSION 3.16)
project(cpp_async_logger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Thread support
find_package(Threads REQUIRED)

#Static Library
add_library(AsyncLoggerLib STATIC
        src/AsyncLogger.cpp
)

target_include_directories(AsyncLoggerLib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(AsyncLoggerLib PUBLIC
        Threads::Threads
)

#Example application
add_executable(AsyncLoggerApp
        src/main.cpp
)

target_link_libraries(AsyncLoggerApp PRIVATE
    AsyncLoggerLib
)