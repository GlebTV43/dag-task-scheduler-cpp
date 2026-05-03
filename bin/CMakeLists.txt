set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME} 
                main.cpp
                ../src/requesters/GeoRequester.cpp
                ../src/requesters/WeatherRequester.cpp
                ../src/requesters/PlaceRequester.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    cpr::cpr
    nlohmann_json::nlohmann_json
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    PROJECT_ROOT="${PROJECT_SOURCE_DIR}"
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
