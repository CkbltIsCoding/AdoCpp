include(FetchContent)
message(STATUS "Getting RapidJSON...")
FetchContent_Declare(RapidJSON
        GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
        GIT_TAG v1.1.0
        GIT_SHALLOW ON
        SYSTEM
#        FIND_PACKAGE_ARGS NAMES RapidJSON
)
FetchContent_MakeAvailable(RapidJSON)
add_library(rapidjson INTERFACE)
add_library(rapidjson::rapidjson ALIAS rapidjson)
target_include_directories(rapidjson INTERFACE ${RapidJSON_SOURCE_DIR}/include)
