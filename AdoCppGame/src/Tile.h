// NOLINT(*-narrowing-conversions)
#pragma once

#include <AdoCpp.h>
#include <SFML/Graphics.hpp>
#include <SelbaWard/Polygon.hpp>
#include <SelbaWard/Spline.hpp>
// #include <cmath>

class TileShape final : public sw::Polygon
{
    static constexpr float pi = 3.14159265f;
    static constexpr float w = 0.273f, l = 0.5f;

    // static float L(float a, float b, float t) { return a + t * (b - a); }
    // static float q(float x)
    // {
    //     return x <= 5 ? 1
    //         : x <= 30 ? L(1, 0.83f, std::sqrt((x - 5) / 25))
    //         : x <= 45 ? L(0.83f, 0.77f, (x - 30) / 15)
    //         : x <= 90 ? L(0.77f, 0.15f, std::pow((x - 45) / 45, 0.7f))
    //                   : L(0.15f, 0, std::sqrt((x - 90) / 45));
    // }
    // static float f(float x) { return x <= pi / 36 ? 0 : -(L(0, w, q(x * 180 / pi)) - w) / std::sin(x / 2); }

public:
    TileShape() = default;
    TileShape(double l_lastAngle, double l_angle, const double l_nextAngle);
    ~TileShape() override = default;
    // void update2()
    // {
    //     // const double m_angle2 = m_angle == 999 ? m_lastAngle + 180 : m_angle;
    //     // const float a1 = (float)m_angle2, a2 = (float)m_nextAngle;
    //     // const float alpha = pi / 180 * std::min(fmod(a1 - a2, 360), fmod(a2 - a1, 360));
    //     // const float
    // }
    // ReSharper disable once CppHidingFunction
    void update();
    double getAngle() const { return m_angle; }
    void setAngle(double l_angle) { m_angle = l_angle; }
    double getNextAngle() const { return m_nextAngle; }
    void setNextAngle(double l_nextAngle) { m_nextAngle = l_nextAngle; }
    void setCircleInterpolationLevel(int l_interpolationLevel) { m_interpolationLevel = l_interpolationLevel; }

private:
    double m_lastAngle{}, m_angle{}, m_nextAngle{};
    int m_interpolationLevel{16};
};

class TileSprite final : public sf::Drawable, public sf::Transformable
{
public:
    TileSprite() = default;
    TileSprite(double lastAngleDeg, double angleDeg, double nextAngleDeg);

    sf::FloatRect getGlobalBounds() const { return m_shape.getGlobalBounds(); }

    sf::FloatRect getGlobalBoundsFaster() const { return getTransform().transformRect({{-0.5, -0.5}, {1, 1}}); }

    double getAngle() const { return m_angleDeg; }

    bool getTwirl() const { return m_twirl; }
    void setTwirl(bool twirl)
    {
        if (m_twirl == twirl)
            return;
        m_twirl = twirl;
        m_needToUpdate = true;
    }
    int getSpeed() const { return m_speed; }
    void setSpeed(int speed)
    {
        if (m_speed == speed)
            return;
        m_speed = speed;
        m_needToUpdate = true;
    }

    sf::Color getTrackColor() const { return m_trackColor; }
    void setTrackColor(sf::Color trackColor)
    {
        if (m_trackColor == trackColor)
            return;
        m_trackColor = trackColor;
        m_needToUpdate = true;
    }

    AdoCpp::TrackStyle getTrackStyle() const { return m_trackStyle; }
    void setTrackStyle(AdoCpp::TrackStyle trackStyle)
    {
        if (m_trackStyle == trackStyle)
            return;
        m_trackStyle = trackStyle;
        m_needToUpdate = true;
    }
    void setOpacity(float opacity)
    {
        if (m_opacity == opacity)
            return;
        m_opacity = opacity;
        m_needToUpdate = true;
    }
    void setActive(bool active)
    {
        if (m_active == active)
            return;
        m_active = active;
        m_needToUpdate = true;
    }

    void update();

    bool isPointInside(const sf::Vector2f point) const
    {
        return m_shape.isPointInside(getInverseTransform().transformPoint(point));
    }

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    TileShape m_shape;
    sw::Spline m_outline;
    sf::CircleShape m_twirlShape;
    sf::CircleShape m_speedShape;

    bool m_needToUpdate;
    bool m_twirl;
    bool m_active;
    int m_speed;
    sf::Color m_trackColor;
    AdoCpp::TrackStyle m_trackStyle;
    float m_opacity;
    double m_angleDeg;
    double m_nextAngleDeg;
};

class TileSystem final : public sf::Drawable
{
public:
    explicit TileSystem(AdoCpp::Level& l_level) : m_level(l_level) { parse(); }
    void parse();
    void setActiveTileIndex(const std::optional<size_t> i) { m_activeTileIndex = i; }
    void update();
    // ReSharper disable once CppMemberFunctionMayBeConst
    TileSprite& operator[](const size_t index)  { return m_tileSprites[index]; }

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    AdoCpp::Level& m_level;
    std::optional<size_t> m_activeTileIndex;
    mutable std::vector<TileSprite> m_tileSprites;
};
