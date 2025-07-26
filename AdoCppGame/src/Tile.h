// NOLINT(*-narrowing-conversions)
#pragma once

#include <AdoCpp.h>
#include <SFML/Graphics.hpp>
#include <SelbaWard/Polygon.hpp>
#include <SelbaWard/Spline.hpp>
#include <cmath>

class TileShape final : public sf::Drawable, public sf::Transformable
{
    static constexpr float pi = 3.14159265f;
    static constexpr float w = 0.273f, l = 0.5f;

public:
    TileShape() = default;
    TileShape(double l_lastAngle, double l_angle, double l_nextAngle);
    ~TileShape() override = default;
    void update();
    sf::FloatRect getLocalBounds() const;
    sf::FloatRect getGlobalBounds() const;
    double getAngle() const { return m_angle; }
    void setAngle(double l_angle) { m_angle = l_angle; }
    double getNextAngle() const { return m_nextAngle; }
    void setNextAngle(double l_nextAngle) { m_nextAngle = l_nextAngle; }
    void setCircleInterpolationLevel(int l_interpolationLevel) { m_interpolationLevel = l_interpolationLevel; }
    bool isPointInside(sf::Vector2f point) const;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    sf::Color getFillColor() const;
    void setFillColor(sf::Color color);
    sf::Color getOutlineColor() const;
    void setOutlineColor(sf::Color color);

private:
    double m_lastAngle{}, m_angle{}, m_nextAngle{};
    int m_interpolationLevel{16};
    sf::VertexArray m_vertices{};
    std::vector<bool> m_borders{};
    sf::FloatRect m_bounds{};
    sf::Color m_fillColor{}, m_outlineColor{};
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
    // sw::Spline m_outline;
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
    void setTilePlaceMode(const int mode) { m_tilePlaceMode = mode; }
    void update();
    // ReSharper disable once CppMemberFunctionMayBeConst
    TileSprite& operator[](const size_t index) { return m_tileSprites[index]; }

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    AdoCpp::Level& m_level;
    std::optional<size_t> m_activeTileIndex;
    mutable std::vector<TileSprite> m_tileSprites;
    int m_tilePlaceMode;
    sf::Font font{"assets/font/Maplestory OTF Bold.otf"};
};
