#pragma once

#include <array>

class Flag
{
	public:
		enum Attribute
		{
			// numeric
			BARS,
			STRIPES,
			COLOURS,
			// bool
			RED_PRESENT,
			GREEN_PRESENT,
			BLUE_PRESENT,
			GOLD_PRESENT,
			WHITE_PRESENT,
			BLACK_PRESENT,
			ORANGE_PRESENT,
			// string
			MAIN_HUE,
			//numeric
			CIRCLES,
			CROSSES,
			SALTIRES,
			QUARTERS,
			SUNSTARS,
			// bool
			CRESENT,
			TRIANGLE,
			ICON,
			ANIMATE,
			// string
			TEXT,
			TOP_LEFT,
			BOT_RIGHT
		};

		static constexpr int AttributeCount = BOT_RIGHT + 1;

		Flag(const std::string& countryName);
		void setAttribute(Attribute attr, int value);
		int getAttribute(Attribute attr) const;

		static bool numericAttribute(Attribute attr);

	private:
		std::string name;
		std::array<int, AttributeCount> attributes;
};
