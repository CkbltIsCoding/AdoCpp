include (FetchContent)

message(STATUS "Getting SelbaWard...")

FetchContent_Declare(
    selbaward
    GIT_REPOSITORY https://github.com/Hapaxia/SelbaWard.git
)
FetchContent_GetProperties(selbaward)
if (NOT selbaward_POPULATED)
    FetchContent_Populate(selbaward)
endif()
