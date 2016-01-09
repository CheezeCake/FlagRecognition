#include "colors.hpp"

const std::tuple<Colors::Range, Colors::Range, Colors::Range> Colors::colorRanges[] = {
	[Color::RED] = std::make_tuple(std::make_pair(0, 180), std::make_pair(230, 255), std::make_pair(60, 255)),
	[Color::GREEN] = std::make_tuple(std::make_pair(38, 75), std::make_pair(150, 255), std::make_pair(60, 255)),
	[Color::BLUE] = std::make_tuple(std::make_pair(75, 135), std::make_pair(150, 255), std::make_pair(60, 255)),
	[Color::GOLD] = std::make_tuple(std::make_pair(22, 38), std::make_pair(150, 255), std::make_pair(60, 255)),
	[Color::WHITE] = std::make_tuple(std::make_pair(0, 200), std::make_pair(0, 10), std::make_pair(0, 255)),
	[Color::BLACK] = std::make_tuple(std::make_pair(0, 10), std::make_pair(0, 100), std::make_pair(0, 20)),
	[Color::ORANGE] = std::make_tuple(std::make_pair(0, 22), std::make_pair(150, 255), std::make_pair(60, 100)),
};

const std::map<std::string, Colors::Color> Colors::colorCodes = {
	{ "red", Color::RED },
	{ "green", Color::GREEN },
	{ "blue", Color::BLUE },
	{ "gold", Color::GOLD },
	{ "white", Color::WHITE },
	{ "black", Color::BLACK },
	{ "orange", Color::ORANGE }
};

int Colors::getColorCode(uint8_t h, uint8_t s, uint8_t v)
{
	for (int i = 0; i <= ORANGE; i++) {
		if (h >= std::get<0>(colorRanges[i]).first && h <= std::get<0>(colorRanges[i]).second &&
				s >= std::get<1>(colorRanges[i]).first && s <= std::get<1>(colorRanges[i]).second &&
				v >= std::get<2>(colorRanges[i]).first && v <= std::get<2>(colorRanges[i]).second)
			return i;
	}

	return -1;
}

int Colors::getColorCode(const std::string& color)
{
	try {
		return colorCodes.at(color);
	}
	catch (const std::out_of_range&) {
		return -1;
	}
}
