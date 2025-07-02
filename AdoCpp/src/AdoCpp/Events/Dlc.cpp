#include "Dlc.h"

namespace AdoCpp::Event::Dlc
{
    Hold::Hold(const rapidjson::Value& data) : StaticEvent(data)
    {
        duration = data["duration"].GetDouble();
        distanceMultiplier = data["distanceMultiplier"].GetDouble();
        landingAnimation = data["landingAnimation"].GetBool();
    }
} // namespace AdoCpp::Event::Dlc
