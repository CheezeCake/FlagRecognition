#pragma once

#include <tuple>


class Colors
{
	public:
		typedef std::pair<uint8_t, uint8_t> Range;

		enum Color
		{
			RED,
			GREEN,
			BLUE,
			GOLD, // also yellow
			WHITE,
			BLACK,
			ORANGE
		};

		static int getColorCode(uint8_t h, uint8_t s, uint8_t v);

	private:
		static const std::tuple<Range, Range, Range> colorRanges[Colors::ORANGE + 1];
};
