#include "colors.hpp"

const std::vector<std::tuple<Colors::Range, Colors::Range, Colors::Range>> Colors::colorRanges[] = {
	// 2 color ranges for RED
	[Color::RED] = {
		std::make_tuple(std::make_pair(0, 5), std::make_pair(200, 255), std::make_pair(100, 255)),
		std::make_tuple(std::make_pair(169, 179), std::make_pair(200, 255), std::make_pair(100, 255))
	},
	[Color::GREEN] = {
		std::make_tuple(std::make_pair(49, 98), std::make_pair(200, 255), std::make_pair(100, 255))
	},
	[Color::BLUE] = {
		std::make_tuple(std::make_pair(98, 122), std::make_pair(200, 255), std::make_pair(100, 255))
	},
	[Color::GOLD] = {
		std::make_tuple(std::make_pair(20, 33), std::make_pair(200, 255), std::make_pair(100, 255))
	},
	[Color::WHITE] = {
		std::make_tuple(std::make_pair(0, 255), std::make_pair(0, 18), std::make_pair(237, 255))
	},
	[Color::BLACK] = {
		std::make_tuple(std::make_pair(0, 255), std::make_pair(0, 255), std::make_pair(0, 38))
	},
	[Color::ORANGE] = {
		std::make_tuple(std::make_pair(10, 20), std::make_pair(200, 255), std::make_pair(100, 100))
	}
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
		for (const auto& range : colorRanges[i]) {
			if (h >= std::get<0>(range).first && h <= std::get<0>(range).second &&
					s >= std::get<1>(range).first && s <= std::get<1>(range).second &&
					v >= std::get<2>(range).first && v <= std::get<2>(range).second)
				return i;
		}
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
