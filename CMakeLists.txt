cmake_minimum_required(VERSION 3.16)
project(ABS_Simulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ABS_CORE_SOURCES
    src/Sensor.cpp
    src/SlipRatio.cpp
    src/ABSController.cpp
    src/FaultDetection.cpp
    src/Watchdog.cpp
)

add_library(abs_core ${ABS_CORE_SOURCES})
target_include_directories(abs_core PUBLIC include)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(abs_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(abs_simulation src/main.cpp)
target_link_libraries(abs_simulation PRIVATE abs_core)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(abs_simulation PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(abs_tests
    tests/test_main.cpp
    tests/test_sensor.cpp
    tests/test_slip_ratio.cpp
    tests/test_abs_controller.cpp
    tests/test_fault_detection.cpp
    tests/test_watchdog.cpp
)
target_include_directories(abs_tests PRIVATE tests)
target_link_libraries(abs_tests PRIVATE abs_core)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(abs_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_test(NAME abs_tests COMMAND abs_tests)
