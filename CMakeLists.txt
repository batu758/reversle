cmake_minimum_required(VERSION 3.28)

project(reversle
        VERSION 1.0
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
        CLI11
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
        GIT_TAG v2.6.2
)

FetchContent_MakeAvailable(CLI11)

add_executable(reversle
        src/main.cpp
        src/feedback.cpp
        src/reader.cpp
        src/solver.cpp
        src/word.cpp
        src/wordle.cpp
)

target_include_directories(reversle PRIVATE include)

target_link_libraries(reversle PRIVATE CLI11::CLI11)

target_compile_options(reversle PRIVATE
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-march=native>
)

set_property(TARGET reversle PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
