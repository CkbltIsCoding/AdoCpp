#pragma once

#include <SFML/Graphics.hpp>
#include <AdoCpp.h>

class KeyViewerSystem : public sf::Drawable, public sf::Transformable
{
public:
	KeyViewerSystem() = default;
	KeyViewerSystem(std::vector<sf::Keyboard::Scan> keyLimiter)
		: m_keyLimiter(keyLimiter)
	{
		for (auto& key : m_keyLimiter)
		{
			m_keyPressed.push_back(std::list<bool>(500));
		}
	}
	void update(double delta)
	{
		for (size_t i = 0; i < m_keyLimiter.size(); i++)
		{
			for (size_t j = 0; j < delta; j++)
			{
				m_keyPressed[i].pop_front();
				m_keyPressed[i].push_back(sf::Keyboard::isKeyPressed(m_keyLimiter[i]));
			}
		}
	}
private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		static const sf::Color white = sf::Color::White,
			lightgray = sf::Color(200, 200, 200);
		states.transform *= getTransform();
		states.texture = nullptr;
		sf::RectangleShape rect{ {10.f, 1.f} };
		sf::RectangleShape sqrPrs{ {10.f, 10.f} };
		sf::RectangleShape sqrRls{ {10.f, 10.f} };
		rect.setFillColor(sf::Color::White);
		sqrRls.setFillColor(sf::Color::Transparent);
		for (size_t i = 0; i < m_keyLimiter.size(); i++)
		{
			if (i % 2) rect.setFillColor(white), sqrPrs.setFillColor(white);
			else rect.setFillColor(lightgray), sqrPrs.setFillColor(lightgray);

			if (m_keyPressed[i].back())
			{
				sqrPrs.setPosition({ float(i * 10), m_keyPressed[i].size() / 5.f + 2.f });
				target.draw(sqrPrs, states);
			}
			else
			{
				sqrRls.setPosition({ float(i * 10), m_keyPressed[i].size() / 5.f + 2.f });
				target.draw(sqrRls, states);
			}
			size_t j = 0;
			for (auto& k : m_keyPressed[i])
			{
				if (k)
				{
					rect.setPosition({ float(i * 10), j / 5.f });
					target.draw(rect, states);
				}
				j++;
			}
		}
	}
	std::vector<sf::Keyboard::Scan> m_keyLimiter;
	std::vector<std::list<bool>> m_keyPressed;
};