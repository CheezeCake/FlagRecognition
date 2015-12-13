#include "colors.hpp"

const std::tuple<Colors::Range, Colors::Range, Colors::Range> Colors::colorRanges[] = {
	[Colors::RED] = std::make_tuple(std::make_pair(0, 179), std::make_pair(230, 255), std::make_pair(230, 255)),
	[Colors::GREEN] = std::make_tuple(std::make_pair(28, 75), std::make_pair(230, 255), std::make_pair(230, 255)),
	[Colors::BLUE] = std::make_tuple(std::make_pair(75, 130), std::make_pair(150, 255), std::make_pair(150, 255)),
	[Colors::GOLD] = std::make_tuple(std::make_pair(22, 38), std::make_pair(230, 255), std::make_pair(230, 255)),
	[Colors::WHITE] = std::make_tuple(std::make_pair(0, 200), std::make_pair(0, 10), std::make_pair(0, 255)),
	[Colors::BLACK] = std::make_tuple(std::make_pair(0, 10), std::make_pair(0, 100), std::make_pair(0, 20)),
	[Colors::ORANGE] = std::make_tuple(std::make_pair(25, 40), std::make_pair(90, 100), std::make_pair(90, 100)),
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
