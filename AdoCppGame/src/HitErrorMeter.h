#pragma once

#include <SFML/Graphics.hpp>
#include <AdoCpp.h>

class HitErrorMeterSystem : public sf::Drawable, public sf::Transformable
{
public:
	HitErrorMeterSystem()
	{
		static const sf::Color
			green = sf::Color::Green,
			yellow = sf::Color::Yellow,
			orange = sf::Color(0xff8800ff),
			red = sf::Color::Red;
		float pw = 30, lepw = 45, vlew = 60, tlew = 65, h = 5;
		redRect.setPosition({ -tlew / 2, 0 }), redRect.setSize({ tlew, h }), redRect.setFillColor(red);
		orangeRect.setPosition({ -vlew / 2, 0 }), orangeRect.setSize({ vlew, h }), orangeRect.setFillColor(orange);
		yellowRect.setPosition({ -lepw / 2, 0 }), yellowRect.setSize({ lepw, h }), yellowRect.setFillColor(yellow);
		greenRect.setPosition({ -pw / 2, 0 }), greenRect.setSize({ pw, h }), greenRect.setFillColor(green);
	}

	void clear()
	{
		m_ticks.clear();
	}
	
	void update(double timer)
	{
		for (auto it = m_ticks.begin(); it < m_ticks.end(); )
		{
			float elapsed = float(timer - it->timerBegin);
			if (elapsed > 4000)
			{
				it = m_ticks.erase(it);
			}
			else
			{
				it->opacity = 1 - elapsed / 4000;
				it++;
			}
		}
	}
	void addTick(
		double l_timer,
		AdoCpp::HitMargin l_hitMargin,
		double l_x)
	{
		m_ticks.push_back({ l_timer, l_hitMargin, l_x });
	}
private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.transform *= getTransform();
		states.texture = nullptr;
		target.draw(redRect, states), target.draw(orangeRect, states),
			target.draw(yellowRect, states), target.draw(greenRect, states);
		static const sf::Color
			green = sf::Color::Green,
			yellow = sf::Color::Yellow,
			orange = sf::Color(0xff8800ff),
			red = sf::Color::Red;
		sf::RectangleShape tickRect{ {2.f, 4.f} };

		for (auto& tick : m_ticks)
		{
			tickRect.setPosition({ (float)tick.x, 0.f });
			tickRect.setOrigin({ 1.f, 2.f });
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
			color.a = uint8_t(tick.opacity * 255);
			tickRect.setFillColor(color);
			target.draw(tickRect, states);
		}
	}
	struct Tick
	{
		double timerBegin;
		AdoCpp::HitMargin hitMargin;
		double x;
		double opacity;
	};
	std::vector<Tick> m_ticks;
	sf::RectangleShape redRect, orangeRect, yellowRect, greenRect;
};