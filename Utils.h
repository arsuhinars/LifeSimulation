#pragma once

#include "imgui.h"
#include <SFML/Graphics.hpp>

class Utils
{
public:
	// Функция смешивания цветов
	static sf::Color mixColors(sf::Color a, sf::Color b, float alpha) {
		sf::Color output;
		output.r = static_cast<sf::Uint8>(a.r * (1.0f - alpha) + b.r * alpha);
		output.g = static_cast<sf::Uint8>(a.g * (1.0f - alpha) + b.g * alpha);
		output.b = static_cast<sf::Uint8>(a.b * (1.0f - alpha) + b.b * alpha);
		return output;
	}

	template<typename T>
	static T mod(T a, T b) {
		return ((b)+(a) % (b)) % (b);
	}
	
	template<typename T>
	static T clamp(T x, T min, T max) {
		if (x > max)
			return max;
		else if (x < min)
			return min;
		else
			return x;
	}

	static sf::Color hsvToRgb(float h, float s, float v) {
		float r, g, b;
		ImGui::ColorConvertHSVtoRGB(h, s, v, r, g, b);
		return sf::Color(
			static_cast<sf::Uint8>(r),
			static_cast<sf::Uint8>(g),
			static_cast<sf::Uint8>(b)
		);
	}
};