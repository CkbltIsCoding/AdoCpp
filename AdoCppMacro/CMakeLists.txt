add_executable(AdoCppMacro src/main.cpp)

target_include_directories(
        AdoCppMacro PRIVATE
        ${PROJECT_SOURCE_DIR}/AdoCpp/src
)

add_dependencies (AdoCppMacro AdoCpp)
target_link_libraries (
        AdoCppMacro PRIVATE
        rapidjson::rapidjson
        AdoCpp
)