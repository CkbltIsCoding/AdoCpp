include(FetchContent)
message(STATUS "Getting Exprtk...")
FetchContent_Declare(
    exprtk
    GIT_REPOSITORY "https://github.com/ArashPartow/exprtk"
)
FetchContent_MakeAvailable(exprtk)
