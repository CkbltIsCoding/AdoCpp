include (FetchContent)

message(STATUS "Getting ImGui & ImGui-SFML & ImGuiFileDialog...")

set (IMGUI_VERSION 1.91.8)
set (IMGUI_SFML_VERSION 3.0)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v${IMGUI_VERSION}
)

FetchContent_GetProperties(imgui)
if (NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
    FetchContent_MakeAvailable(imgui)
    
endif()


set(IMGUI_DIR ${imgui_SOURCE_DIR})
option(IMGUI_SFML_FIND_SFML "Use find_package to find SFML" OFF)
option(IMGUI_SFML_IMGUI_DEMO "Build imgui_demo.cpp" ON)

FetchContent_Declare(
    imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG        v${IMGUI_SFML_VERSION}
)

FetchContent_GetProperties(imgui-sfml)
if (NOT imgui-sfml_POPULATED)
    FetchContent_Populate(imgui-sfml)
    FetchContent_MakeAvailable(imgui-sfml)
endif()

FetchContent_Declare(
    igfd
    GIT_REPOSITORY https://github.com/aiekick/ImGuiFileDialog.git
)

FetchContent_GetProperties(igfd)
if (NOT igfd_POPULATED)
    FetchContent_Populate(igfd)
    FetchContent_MakeAvailable(igfd)
endif()
