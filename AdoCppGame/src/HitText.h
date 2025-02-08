#pragma once

#include <SFML/Graphics.hpp>
#include <AdoCpp.h>
#include <array>

class TooEarlySprite : public sf::Drawable, public sf::Transformable
{
public:
	TooEarlySprite()
	{
		circle.setFillColor(sf::Color::White);
		circle.setRadius(0.125f);
		circle.setOrigin({ 0.125f, 0.125f });
		rect1.setFillColor(sf::Color::Red);
		rect1.setSize({ 0.05f, 0.2f });
		rect1.setOrigin({ 0.025f, 0.1f });
		rect1.setRotation(sf::degrees(45));
		rect2.setFillColor(sf::Color::Red);
		rect2.setSize({ 0.05f, 0.2f });
		rect2.setOrigin({ 0.025f, 0.1f });
		rect2.setRotation(sf::degrees(135));
	}

	~TooEarlySprite()
	{
	}

	void setOpacity(float opacity)
	{
		circle.setFillColor(sf::Color::White * sf::Color(255, 255, 255, uint8_t(opacity * 255)));
		rect1.setFillColor(sf::Color::Red * sf::Color(255, 255, 255, uint8_t(opacity * 255)));
		rect2.setFillColor(sf::Color::Red * sf::Color(255, 255, 255, uint8_t(opacity * 255)));
	}
	
private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.transform *= getTransform();
		states.texture = nullptr;
		target.draw(circle, states);
		target.draw(rect1, states);
		target.draw(rect2, states);
	}
	sf::CircleShape circle;
	sf::RectangleShape rect1;
	sf::RectangleShape rect2;
};



class HitTextSystem : public sf::Drawable, public sf::Transformable
{
public:
	HitTextSystem(sf::Font &l_font) : m_font(l_font)
	{
		setFont(l_font);
	}

	void clear()
	{
		m_hitTexts.clear();
	}

	void update(double timer)
	{
		for (auto it = m_hitTexts.begin(); it < m_hitTexts.end(); )
		{
			float elapsed = float(timer - it->timerBegin);
			if (elapsed > 4000)
			{
				it = m_hitTexts.erase(it);
			}
			else
			{
				it->scale = { 1 - elapsed / 4000, 1 - elapsed / 4000 };
				it->opacity = 1 - elapsed / 4000;
				if (it->hitMargin == AdoCpp::HitMargin::TooEarly)
				{
					it->tes.setPosition(it->position);
					it->tes.setOpacity(1 - ((int)elapsed % 1000) / 1000.f);
				}
				it++;
			}
		}
	}
	void setFont(sf::Font l_font)
	{
		m_font = l_font;
		for (size_t i = 0; i < 7; i++)
		{
			m_text[i].setCharacterSize(32);
		}
		static const sf::Color
			green = sf::Color::Green,
			yellow = sf::Color::Yellow,
			orange = sf::Color(0xff8800ff),
			red = sf::Color::Red;
		m_text[(int)AdoCpp::HitMargin::Perfect].setFillColor(green);
		m_text[(int)AdoCpp::HitMargin::Perfect].setString("Perfect!");
		m_text[(int)AdoCpp::HitMargin::LatePerfect].setFillColor(yellow);
		m_text[(int)AdoCpp::HitMargin::LatePerfect].setString("LPerfect!");
		m_text[(int)AdoCpp::HitMargin::EarlyPerfect].setFillColor(yellow);
		m_text[(int)AdoCpp::HitMargin::EarlyPerfect].setString("EPerfect!");
		m_text[(int)AdoCpp::HitMargin::VeryLate].setFillColor(orange);
		m_text[(int)AdoCpp::HitMargin::VeryLate].setString("Late!");
		m_text[(int)AdoCpp::HitMargin::VeryEarly].setFillColor(orange);
		m_text[(int)AdoCpp::HitMargin::VeryEarly].setString("Early!");
		m_text[(int)AdoCpp::HitMargin::TooLate].setFillColor(red);
		m_text[(int)AdoCpp::HitMargin::TooLate].setString("Late!!");
		m_text[(int)AdoCpp::HitMargin::TooEarly].setFillColor(red);
		m_text[(int)AdoCpp::HitMargin::TooEarly].setString("Early!!");

	}
	void addHitText(
		double l_timer,
		AdoCpp::HitMargin l_hitMargin,
		sf::Vector2f l_position)
	{
		m_hitTexts.push_back({ l_timer, l_hitMargin, l_position });
	}
	bool hidePerfects = true;
private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.transform *= getTransform();
		states.texture = nullptr;

		sf::Vector2f viewCenter(target.getView().getCenter());
		sf::Vector2f viewSize(target.getView().getSize());
		viewSize.x = viewSize.y = (std::max)(viewSize.x, viewSize.y) * 1.5f;
		sf::FloatRect currentViewRect(viewCenter - viewSize / 2.f, viewSize);
		for (auto& hitText : m_hitTexts)
		{
			if (hidePerfects && hitText.hitMargin == AdoCpp::HitMargin::Perfect) continue;
			sf::Text text = m_text[(int)hitText.hitMargin];
			text.setPosition(hitText.position + sf::Vector2f({0.f, 1.f}));
			text.setOrigin(text.getLocalBounds().size / 2.f);
			sf::Vector2f scale = hitText.scale * 0.01f;
			scale.y *= -1;
			text.setScale(scale);
			sf::Color color = text.getFillColor();
			color.a = std::uint8_t(hitText.opacity * 255);
			text.setFillColor(color);
			if (currentViewRect.findIntersection(text.getGlobalBounds()))
			{
				if (hitText.hitMargin == AdoCpp::HitMargin::TooEarly)
					target.draw(hitText.tes, states);
				target.draw(text, states);
			}
		}
	}

	struct HitText
	{
		double timerBegin;
		AdoCpp::HitMargin hitMargin;
		sf::Vector2f position;
		sf::Vector2f scale;
		float opacity;
		TooEarlySprite tes;
	};
	
	std::vector<HitText> m_hitTexts;
	sf::Font &m_font;
	std::array<sf::Text, 7> m_text = {sf::Text(m_font), sf::Text(m_font), sf::Text(m_font),
	sf::Text(m_font), sf::Text(m_font), sf::Text(m_font), sf::Text(m_font) };
	struct
	{
		sf::CircleShape circle;
		sf::RectangleShape rect1;
		sf::RectangleShape rect2;
	} m_tooEarlyShape;
};