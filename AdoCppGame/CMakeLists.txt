include (${PROJECT_SOURCE_DIR}/GetSelbaWard.cmake)
include (${PROJECT_SOURCE_DIR}/GetRapidjson.cmake)

include (${PROJECT_SOURCE_DIR}/GetImGui.cmake)
file (
	GLOB IMGUI_SRC
	${imgui_SOURCE_DIR}/*.h
	${imgui_SOURCE_DIR}/*.cpp
)
file (
	GLOB IMGUI_SFML_SRC
	${imgui-sfml_SOURCE_DIR}/*.h
	${imgui-sfml_SOURCE_DIR}/*.cpp
)
file (
	GLOB IGFD_SRC
	${igfd_SOURCE_DIR}/*.h
	${igfd_SOURCE_DIR}/*.cpp
)

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
	"src/ImGuiFileDialogConfig.h"
	"src/KeyViewer.h" "src/KeyViewer.cpp"
	
	"${selbaward_SOURCE_DIR}/src/SelbaWard/Polygon.cpp"
	"${selbaward_SOURCE_DIR}/src/SelbaWard/Spline.cpp"
    
	"thirdparty/IconsFontAwesome6.h"

	${IMGUI_SRC}
	${IMGUI_SFML_SRC}
	${IGFD_SRC}
	"src/AudioProcessing.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET AdoCppGame PROPERTY CXX_STANDARD 20)
endif()

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
)

target_link_libraries (AdoCppGame PRIVATE rapidjson::rapidjson)

add_dependencies (AdoCppGame AdoCpp)
target_link_libraries (
	AdoCppGame PRIVATE
	AdoCpp
)

include (${PROJECT_SOURCE_DIR}/GetSFML.cmake)
target_link_libraries (
	AdoCppGame PRIVATE
	sfml-system
	sfml-window
	sfml-graphics
	sfml-audio
)

target_include_directories(AdoCppGame PRIVATE ${imgui_SOURCE_DIR})
target_include_directories(AdoCppGame PRIVATE ${imgui-sfml_SOURCE_DIR})
target_include_directories(AdoCppGame PRIVATE ${igfd_SOURCE_DIR})
target_include_directories(AdoCppGame PRIVATE ${selbaward_SOURCE_DIR}/src)

target_compile_definitions(AdoCppGame PRIVATE NOMINMAX)
