include (FetchContent)
message(STATUS "Getting ImGui & ImGui-SFML & ImGuiFileDialog...")
set (IMGUI_VERSION 1.91.9)
set (IMGUI_SFML_VERSION 3.0)
FetchContent_Declare(
        ImGui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        v${IMGUI_VERSION}
)
FetchContent_MakeAvailable(ImGui)
set(IMGUI_DIR ${imgui_SOURCE_DIR})
#option(IMGUI_SFML_FIND_SFML "Use find_package to find SFML" OFF)
option(IMGUI_SFML_IMGUI_DEMO "Build imgui_demo.cpp" ON)
FetchContent_Declare(
        imgui-sfml
        GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
        GIT_TAG        v${IMGUI_SFML_VERSION}
        FIND_PACKAGE_ARGS NAMES imgui_sfml
)
FetchContent_MakeAvailable(imgui-sfml)
#FetchContent_Declare(
#        igfd
#        GIT_REPOSITORY https://github.com/aiekick/ImGuiFileDialog.git
#        FIND_PACKAGE_ARGS NAMES igfd
#)
#FetchContent_MakeAvailable(igfd)
