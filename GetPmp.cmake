include(FetchContent)
message(STATUS "Getting Pmp-library...")
FetchContent_Declare(
        pmp-library
        GIT_REPOSITORY https://github.com/pmp-library/pmp-library
        GIT_TAG 3.0.0
)
#FetchContent_MakeAvailable(pmp-library)
