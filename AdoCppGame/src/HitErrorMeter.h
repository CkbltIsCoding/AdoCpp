#pragma once

#include <AdoCpp.h>
#include <SFML/Graphics.hpp>

class HitErrorMeterSystem final : public sf::Drawable, public sf::Transformable
{
public:
    HitErrorMeterSystem()
    {
        const sf::Color green = sf::Color::Green, yellow = sf::Color::Yellow, orange = sf::Color(0xff8800ff),
                        red = sf::Color::Red;
        constexpr float pW = 30, lepW = 45, vleW = 60, tleW = 65, h = 5;
        redRect.setPosition({-tleW / 2, 0}), redRect.setSize({tleW, h}), redRect.setFillColor(red);
        orangeRect.setPosition({-vleW / 2, 0}), orangeRect.setSize({vleW, h}), orangeRect.setFillColor(orange);
        yellowRect.setPosition({-lepW / 2, 0}), yellowRect.setSize({lepW, h}), yellowRect.setFillColor(yellow);
        greenRect.setPosition({-pW / 2, 0}), greenRect.setSize({pW, h}), greenRect.setFillColor(green);
    }

    void clear() { m_ticks.clear(); }

    void update(const double seconds)
    {
        for (auto it = m_ticks.begin(); it < m_ticks.end();)
        {
            const auto elapsed = static_cast<float>(seconds - it->secondsBegin);
            if (elapsed > 4)
            {
                it = m_ticks.erase(it);
            }
            else
            {
                it->opacity = 1 - AdoCpp::ease(AdoCpp::Easing::OutCubic, elapsed / 4);
                ++it;
            }
        }
    }
    void addTick(const double seconds, const AdoCpp::HitMargin hitMargin, const double x)
    {
        m_ticks.push_back({seconds, hitMargin, x});
    }

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        states.texture = nullptr;
        target.draw(redRect, states), target.draw(orangeRect, states), target.draw(yellowRect, states),
            target.draw(greenRect, states);
        static const sf::Color green = sf::Color::Green, yellow = sf::Color::Yellow, orange = sf::Color(0xff8800ff),
                               red = sf::Color::Red;
        sf::RectangleShape tickRect{{1.f, 4.f}};

        for (auto& tick : m_ticks)
        {
            tickRect.setPosition({static_cast<float>(tick.x), -2.f});
            tickRect.setOrigin({0.5f, 2.f});
            switch (tick.hitMargin)
            {
            case AdoCpp::HitMargin::TooLate:
            case AdoCpp::HitMargin::TooEarly:
                tickRect.setFillColor(red);
                break;
            case AdoCpp::HitMargin::VeryLate:
            case AdoCpp::HitMargin::VeryEarly:
                tickRect.setFillColor(orange);
                break;
            case AdoCpp::HitMargin::LatePerfect:
            case AdoCpp::HitMargin::EarlyPerfect:
                tickRect.setFillColor(yellow);
                break;
            case AdoCpp::HitMargin::Perfect:
                tickRect.setFillColor(green);
                break;
            default:
                break;
            }
            sf::Color color = tickRect.getFillColor();
            color.a = static_cast<uint8_t>(tick.opacity * 255);
            tickRect.setFillColor(color);
            target.draw(tickRect, states);
        }
    }
    struct Tick
    {
        double secondsBegin;
        AdoCpp::HitMargin hitMargin;
        double x;
        double opacity;
    };
    std::vector<Tick> m_ticks;
    sf::RectangleShape redRect, orangeRect, yellowRect, greenRect;
};
