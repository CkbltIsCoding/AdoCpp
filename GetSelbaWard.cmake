include (FetchContent)
message(STATUS "Getting SelbaWard...")
FetchContent_Declare(
        SelbaWard
        GIT_REPOSITORY https://github.com/Hapaxia/SelbaWard.git
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM
        FIND_PACKAGE_ARGS NAMES SelbaWard
)
FetchContent_MakeAvailable(SelbaWard)
