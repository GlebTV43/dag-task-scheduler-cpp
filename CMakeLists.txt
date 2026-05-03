cmake_minimum_required(VERSION 3.20)

project(
    labwork9
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CPR_USE_SYSTEM_CURL OFF CACHE BOOL "" FORCE)

include(FetchContent)

FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.14.2
)

FetchContent_MakeAvailable(cpr)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)

FetchContent_MakeAvailable(nlohmann_json)

set(API_KEY_SOURCE "${CMAKE_SOURCE_DIR}/src/data/2ip_apikey.txt")
set(API_KEY_DESTINATION "${CMAKE_BINARY_DIR}/src/data/2ip_apikey.txt")

if (EXISTS "${API_KEY_SOURCE}")
    configure_file(
        "${API_KEY_SOURCE}"
        "${API_KEY_DESTINATION}"
        COPYONLY
    )
else()
    message(WARNING "data/2ip_apikey.txt not found. Create it from data/2ip_apikey.example.txt")
endif()

set(API_KEY_SOURCE "${CMAKE_SOURCE_DIR}/src/data/geoapify_apikey.txt")
set(API_KEY_DESTINATION "${CMAKE_BINARY_DIR}/src/data/geoapify_apikey.txt")

if (EXISTS "${API_KEY_SOURCE}")
    configure_file(
        "${API_KEY_SOURCE}"
        "${API_KEY_DESTINATION}"
        COPYONLY
    )
else()
    message(WARNING "data/geoapify_apikey.txt not found. Create it from data/geoapify_apikey.example.txt")
endif()

include_directories(lib)

add_subdirectory(bin)

enable_testing()
add_subdirectory(tests)

# Do not add or modify anything below this line
include(tests/coverage.cmake)