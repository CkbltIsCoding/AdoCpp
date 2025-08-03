include(FetchContent)
message(STATUS "Getting SFML...")
if (USE_MIRROR)
    FetchContent_Declare(SFML
            GIT_REPOSITORY https://gh.xmly.dev/https://github.com/SFML/SFML.git
            GIT_TAG 3.0.1
            GIT_SHALLOW ON
            EXCLUDE_FROM_ALL
            SYSTEM
            FIND_PACKAGE_ARGS NAMES SFML
    )
else ()
    FetchContent_Declare(SFML
            GIT_REPOSITORY https://gh.xmly.dev/https://github.com/SFML/SFML.git
            GIT_TAG 3.0.1
            GIT_SHALLOW ON
            EXCLUDE_FROM_ALL
            SYSTEM
            FIND_PACKAGE_ARGS NAMES SFML
    )
endif ()
FetchContent_MakeAvailable(SFML)
