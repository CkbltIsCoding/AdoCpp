add_executable(test test.cpp)

target_include_directories(
        test PRIVATE
        ${PROJECT_SOURCE_DIR}/AdoCpp/src
)

add_dependencies (test AdoCpp)
target_link_libraries (
        test PRIVATE
        rapidjson::rapidjson
        AdoCpp
)