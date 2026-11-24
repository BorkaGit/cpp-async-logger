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

#GoogleTest
include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.18.0
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(googletest)

#Testing
enable_testing()

add_executable(AsyncLoggerTests
        tests/AsyncLoggerTests.cpp
)

target_link_libraries(AsyncLoggerTests PRIVATE
    AsyncLoggerLib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(AsyncLoggerTests)

if (THREADPOOL_ENABLE_TSAN)
    target_compile_options(AsyncLoggerTests PRIVATE
    -fsanitize=thread
    -fno-omit-frame-pointer
    -g
    -01
    )

    target_link_options(AsyncLoggerTests PRIVATE
    -fsanitize=thread
   )
endif()