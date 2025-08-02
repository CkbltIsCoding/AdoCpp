include(FetchContent)
message(STATUS "Getting ImGui...")
set(IMGUI_VERSION 1.91.9)
set(IMGUI_SFML_VERSION 3.0)
FetchContent_Declare(
        ImGui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v${IMGUI_VERSION}
)
FetchContent_MakeAvailable(ImGui)

message(STATUS "Getting ImGui-SFML...")
set(IMGUI_DIR ${imgui_SOURCE_DIR})
option(IMGUI_SFML_IMGUI_DEMO "Build imgui_demo.cpp" ON)
set(IMGUI_SFML_USE_DEFAULT_CONFIG OFF)
set(IMGUI_SFML_CONFIG_DIR /path/to/dir/with/config)
set(IMGUI_SFML_CONFIG_NAME "../../../AdoCppGame/src/ImGuiConfig.h")
FetchContent_Declare(
        imgui-sfml
        GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
        GIT_TAG v${IMGUI_SFML_VERSION}
        FIND_PACKAGE_ARGS NAMES imgui_sfml
)
FetchContent_MakeAvailable(imgui-sfml)
