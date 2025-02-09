include(FetchContent)

message(STATUS "Getting SFML...")

set(SFML_VERSION "3.0.0")

FetchContent_Declare(
    sfml
    GIT_REPOSITORY "https://github.com/SFML/SFML.git"
    GIT_TAG        "${SFML_VERSION}"
)

FetchContent_GetProperties(sfml)
if(NOT sfml_POPULATED)
    FetchContent_Populate(sfml)

    add_subdirectory(${sfml_SOURCE_DIR} ${sfml_BINARY_DIR})
endif()
