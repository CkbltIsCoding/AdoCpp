#include "Base.h"

namespace AdoCpp::Event::Dlc
{
    class Hold final : public StaticEvent
    {
    public:
        Hold() = default;
        explicit Hold(const rapidjson::Value& data);
        bool stackable() override { return false; }
        std::string name() override { return "Hold"; }
        Hold* clone() override { return new Hold(*this); }
        double duration;
        double distanceMultiplier;
        bool landingAnimation;
    };
} // namespace AdoCpp::Event::Dlc
