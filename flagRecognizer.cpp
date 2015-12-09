#include <iostream>
#include <fstream>
#include <stdexcept>
#include <set>
#include <opencv2/imgproc/imgproc.hpp>
#include "flagRecognizer.hpp"
#include "colors.hpp"

using namespace std::placeholders;

FlagRecognizer::FlagRecognizer(const std::string& filename)
{
	attributeExtractionFunctions[Flag::Attribute::BARS] = std::bind(&FlagRecognizer::extractBars, this, _1);
	attributeExtractionFunctions[Flag::Attribute::STRIPES] = std::bind(&FlagRecognizer::extractStripes, this, _1);
	attributeExtractionFunctions[Flag::Attribute::COLOURS] = std::bind(&FlagRecognizer::extractColours, this, _1);

	loadDataSet(filename);
}

std::string FlagRecognizer::recognize(const cv::Mat& src)
{
	std::array<int, Flag::AttributeCount> srcAttributes;

	for (int i = 0; i < Flag::AttributeCount; i++) {
		if (attributeExtractionFunctions[i])
			srcAttributes[i] = attributeExtractionFunctions[i](src);
	}

	return std::string();
}

int FlagRecognizer::extractBars(const cv::Mat& src) const
{
	int n = 0;

	cv::Mat grey;
	cv::cvtColor(src, grey, cv::COLOR_BGR2GRAY);

	cv::Mat edges;
	cv::Canny(grey, edges, 50, 150);

	std::vector<cv::Vec2f> lines;
	cv::HoughLines(edges, lines, 1, CV_PI/180, 120, 0, 0);

	for (size_t i = 0; i < lines.size(); i++)
	{
		float theta = lines[i][1];

		// TODO: check line size?
		if (theta < degreesToRadians(10) || theta > degreesToRadians(170))
			++n;
	}

	return ((n == 0) ? 0 : (n + 1));
}

int FlagRecognizer::extractStripes(const cv::Mat& src) const
{
	int n = 0;

	cv::Mat grey;
	cv::cvtColor(src, grey, cv::COLOR_BGR2GRAY);

	cv::Mat edges;
	cv::Canny(grey, edges, 50, 150);

	std::vector<cv::Vec2f> lines;
	cv::HoughLines(edges, lines, 1, CV_PI/180, 120, 0, 0);

	for (size_t i = 0; i < lines.size(); i++)
	{
		float theta = lines[i][1];

		// TODO: check line size?
		if (theta > degreesToRadians(80) && theta < degreesToRadians(100))
			++n;
	}

	return ((n == 0) ? 0 : (n + 1));
}

int FlagRecognizer::extractColours(const cv::Mat& src) const
{
	cv::Mat hsv;
	cvtColor(src, hsv, cv::COLOR_BGR2HSV);

	std::set<int> colorCodeSet;

	typedef cv::Point3_<uint8_t> Pixel;

	int n = 0;
	std::set<uint32_t> colorSet;

	for (Pixel& p : cv::Mat_<Pixel>(hsv))
		colorCodeSet.insert(Colors::getColorCode(p.x, p.y, p.z));

	return colorCodeSet.size();
}

void FlagRecognizer::loadDataSet(const std::string& filename)
{
	std::ifstream dataSet(filename);

	if (!dataSet)
		throw std::runtime_error("Error opening data set '" + filename + "'");

	while (!dataSet.eof()) {
		std::string line;
		std::getline(dataSet, line);
		if (line.empty())
			continue;

		int start = line.find(',');
		Flag flag(line.substr(0, start));

		//ignore the next 6 attributes
		for (int i = 0; i < 6; i++)
			start = line.find(',', start + 1);

		for (int i = 0; i < Flag::AttributeCount; i++) {
			int oldStart = start;
			start = line.find(',', start + 1);
			std::string s =  line.substr(oldStart + 1, start - oldStart - 1);
			Flag::Attribute attribute = static_cast<Flag::Attribute>(i);
			if (Flag::numericAttribute(attribute))
				flag.setAttribute(attribute, std::stoi(s));
			/* else */
			/* 	flag.setAttribute(attribute, getColorCode(s)); */

			flags.push_back(flag);
		}
	}
}

constexpr double FlagRecognizer::degreesToRadians(double angle)
{
	return ((CV_PI / 180) * angle);
}
