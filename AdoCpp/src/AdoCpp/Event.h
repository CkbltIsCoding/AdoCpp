#pragma once
#include <rapidjson/document.h>

// ReSharper disable CppUnusedIncludeDirective
#include "Easing.h"
#include "Events/Dlc.h"
#include "Events/GamePlay.h"
#include "Events/Modifiers.h"
#include "Events/Track.h"
#include "Events/Visual.h"
#include "Utils.h"
// ReSharper restore CppUnusedIncludeDirective

/**
 * @brief Event namespace.
 */
namespace AdoCpp::Event
{
    Event* newEvent(const rapidjson::Value& json);
}
