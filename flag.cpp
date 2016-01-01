#include <iostream>
#include <limits>
#include <cassert>
#include "flag.hpp"

const std::array<const std::string, Flag::AttributeCount> Flag::AttributeName = {
	"BARS",
	"STRIPES",
	"COLOURS",
	"RED_PRESENT",
	"GREEN_PRESENT",
	"BLUE_PRESENT",
	"GOLD_PRESENT",
	"WHITE_PRESENT",
	"BLACK_PRESENT",
	"ORANGE_PRESENT",
	"MAIN_HUE",
	"CIRCLES",
	"CROSSES",
	"SALTIRES",
	"QUARTERS",
	"SUNSTARS",
	"CRESENT",
	"TRIANGLE",
	"ICON",
	"ANIMATE",
	"TEXT",
	"TOP_LEFT",
	"BOT_RIGHT"
};

Flag::Flag(const std::string& countryName)
	: name(countryName)
{
}

std::string Flag::getName()
{
	return name;
}

void Flag::setAttribute(Attribute attr, int value)
{
	assert(attr >= 0 && attr < AttributeCount);
	attributes[attr] = value;
}

int Flag::getAttribute(Attribute attr) const
{
	assert(attr >= 0 && attr < AttributeCount);
	return attributes[attr];
}

bool Flag::numericAttribute(Attribute attr)
{
	switch (attr) {
		case BARS:
		case STRIPES:
		case COLOURS:
		case RED_PRESENT:
		case GREEN_PRESENT:
		case BLUE_PRESENT:
		case GOLD_PRESENT:
		case WHITE_PRESENT:
		case BLACK_PRESENT:
		case ORANGE_PRESENT:
		case CIRCLES:
		case CROSSES:
		case SALTIRES:
		case QUARTERS:
		case SUNSTARS:
		case CRESENT:
		case TRIANGLE:
		case ICON:
		case ANIMATE:
		case TEXT:
			return true;
		default:
			return false;
	}
}

std::string Flag::getAttributeName(Attribute attr)
{
	assert(attr >= 0 && attr < AttributeCount);
	return AttributeName[attr];
}
