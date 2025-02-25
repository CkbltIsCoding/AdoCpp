include(FetchContent)

message(STATUS "Getting RapidJSON...")

FetchContent_Declare(
    rapidjson
    URL https://github.com/Tencent/rapidjson/archive/v1.1.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)

FetchContent_GetProperties(rapidjson)
if(NOT rapidjson_POPULATED)
  FetchContent_Populate(rapidjson)

  add_library(rapidjson INTERFACE)
  add_library(rapidjson::rapidjson ALIAS rapidjson)
  
  target_include_directories(rapidjson INTERFACE ${rapidjson_SOURCE_DIR}/include)
endif()

# import targets��
# rapidjson::rapidjson