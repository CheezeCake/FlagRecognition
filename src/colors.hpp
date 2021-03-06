#pragma once

#include <string>
#include <map>
#include <tuple>
#include <vector>


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
		static int getColorCode(const std::string& color);

	private:
		static const std::vector<std::tuple<Range, Range, Range>> colorRanges[Colors::ORANGE + 1];
		static const std::map<std::string, Color> colorCodes;
};
