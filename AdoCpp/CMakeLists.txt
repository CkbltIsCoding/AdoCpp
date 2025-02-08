add_library (
	AdoCpp STATIC
	"src/AdoCpp.h"
	"src/AdoCpp/Easing.cpp"
	"src/AdoCpp/Easing.h"
	"src/AdoCpp/Event.cpp"
	"src/AdoCpp/Event.h"
	"src/AdoCpp/Level.cpp"
	"src/AdoCpp/Level.h"
	"src/AdoCpp/Utils.cpp"
	"src/AdoCpp/Utils.h"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET AdoCpp PROPERTY CXX_STANDARD 20)
endif()

include (${PROJECT_SOURCE_DIR}/GetRapidjson.cmake)
target_link_libraries (AdoCpp PRIVATE rapidjson::rapidjson)