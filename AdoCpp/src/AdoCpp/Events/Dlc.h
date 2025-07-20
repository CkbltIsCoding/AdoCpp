#pragma once
#include "Base.h"

namespace AdoCpp::Event::Dlc
{
    class Hold final : public StaticEvent
    {
    public:
        Hold() = default;
        explicit Hold(const rapidjson::Value& data);
        [[nodiscard]] constexpr bool stackable() const noexcept override { return false; }
        [[nodiscard]] constexpr const char* name() const noexcept override { return "Hold"; }
        [[nodiscard]] constexpr Hold* clone() const override { return new Hold(*this); }
        [[nodiscard]] rapidjson::Value intoJson(rapidjson::Document::AllocatorType& alloc) const override;
        double duration = 1;
        double distanceMultiplier = 1;
        bool landingAnimation = false;
    };
} // namespace AdoCpp::Event::Dlc
