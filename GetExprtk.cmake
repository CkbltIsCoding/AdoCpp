include(FetchContent)
message(STATUS "Getting Exprtk...")
if (USE_MIRROR)
    FetchContent_Declare(
            exprtk
            GIT_REPOSITORY https://gh.xmly.dev/https://github.com/ArashPartow/exprtk
    )
else ()
    FetchContent_Declare(
            exprtk
            GIT_REPOSITORY https://github.com/ArashPartow/exprtk
    )
endif ()
FetchContent_MakeAvailable(exprtk)
