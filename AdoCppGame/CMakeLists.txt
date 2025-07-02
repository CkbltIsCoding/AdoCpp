#include(${PROJECT_SOURCE_DIR}/GetRapidJSON.cmake)
include(${PROJECT_SOURCE_DIR}/GetSFML.cmake)
include(${PROJECT_SOURCE_DIR}/GetSelbaWard.cmake)
include(${PROJECT_SOURCE_DIR}/GetImGui.cmake)
include(${PROJECT_SOURCE_DIR}/GetExprtk.cmake)

find_package(SFML REQUIRED COMPONENTS System Window Graphics Audio)
find_package(SelbaWard REQUIRED)
#find_package(igfd REQUIRED)
#find_package(RapidJSON REQUIRED)

#file (
#    GLOB IMGUI_SRC
#    ${imgui_SOURCE_DIR}/*.h
#    ${imgui_SOURCE_DIR}/*.cpp
#)
#file (
#    GLOB IMGUI_SFML_SRC
#    ${imgui-sfml_SOURCE_DIR}/*.h
#    ${imgui-sfml_SOURCE_DIR}/*.cpp
#)

add_executable (
        AdoCppGame
        "src/main.cpp"
        "src/Game.h" "src/Game.cpp"
        "src/AssetManager.h" "src/AssetManager.cpp"
        "src/State.h"
        "src/State/Charting.h" "src/State/Charting.cpp"
        "src/State/Playing.h" "src/State/Playing.cpp"
        "src/Tile.h"
        "src/HitErrorMeter.h"
        "src/HitText.h"
        "src/KeyViewer.h" "src/KeyViewer.cpp"
        "src/AudioProcessing.h"

        "thirdparty/IconsFontAwesome6.h"
        "thirdparty/ImGuiFileDialog.h"
        "thirdparty/ImGuiFileDialog.cpp"
        "thirdparty/ImGuiFileDialogConfig.h"
)

set_property(TARGET AdoCppGame PROPERTY CXX_STANDARD 20)

if(CMAKE_COMPILER_IS_GNUCXX)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-Wa,-mbig-obj" GNU_BIG_OBJ_FLAG_ENABLE)
endif()
target_compile_options(AdoCppGame
        PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/bigobj>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<BOOL:${GNU_BIG_OBJ_FLAG_ENABLE}>>:-Wa,-mbig-obj>
)

# copy assets
add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(AdoCppGame copy_assets)

target_include_directories(
        AdoCppGame PRIVATE
        src
        thirdparty
        ${PROJECT_SOURCE_DIR}/AdoCpp/src
        ${exprtk_SOURCE_DIR}
)

add_dependencies (AdoCppGame AdoCpp)
target_link_libraries (
        AdoCppGame PRIVATE

        rapidjson::rapidjson

        AdoCpp

        SFML::System
        SFML::Window
        SFML::Graphics
        SFML::Audio
        SelbaWard::SelbaWard

        ImGui-SFML::ImGui-SFML
)
#target_include_directories(AdoCppGame PRIVATE ${imgui_SOURCE_DIR})
#target_link_libraries(
#        AdoCppGame PRIVATE
#        imgui_sfml
##        ImGuiFileDialog
#)
target_include_directories(
        AdoCppGame PRIVATE
        #        ${imgui-sfml_SOURCE_DIR}
        ${igfd_SOURCE_DIR}
        #        ${SelbaWard_SOURCE_DIR}/src
)

target_compile_definitions(AdoCppGame PRIVATE NOMINMAX)
