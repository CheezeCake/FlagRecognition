#include <iostream>
#include <fstream>
#include <stdexcept>
#include <set>
#include <map>
#include <opencv2/imgproc/imgproc.hpp>
#include "flagRecognizer.hpp"
#include "colors.hpp"

using namespace std::placeholders;

typedef cv::Point3_<uint8_t> Pixel;

FlagRecognizer::FlagRecognizer(const std::string& filename)
{
	attributeExtractionFunctions[Flag::Attribute::BARS] = std::bind(&FlagRecognizer::extractBars, this, _1);
	attributeExtractionFunctions[Flag::Attribute::STRIPES] = std::bind(&FlagRecognizer::extractStripes, this, _1);
	attributeExtractionFunctions[Flag::Attribute::COLOURS] = std::bind(&FlagRecognizer::extractColours, this, _1);
	attributeExtractionFunctions[Flag::Attribute::RED_PRESENT] = std::bind(&FlagRecognizer::extractRedPresent, this, _1);
	attributeExtractionFunctions[Flag::Attribute::GREEN_PRESENT] = std::bind(&FlagRecognizer::extractGreenPresent, this, _1);
	attributeExtractionFunctions[Flag::Attribute::BLUE_PRESENT] = std::bind(&FlagRecognizer::extractBluePresent, this, _1);
	attributeExtractionFunctions[Flag::Attribute::GOLD_PRESENT] = std::bind(&FlagRecognizer::extractGoldPresent, this, _1);
	attributeExtractionFunctions[Flag::Attribute::WHITE_PRESENT] = std::bind(&FlagRecognizer::extractWhitePresent, this, _1);
	attributeExtractionFunctions[Flag::Attribute::BLACK_PRESENT] = std::bind(&FlagRecognizer::extractBlackPresent, this, _1);
	attributeExtractionFunctions[Flag::Attribute::ORANGE_PRESENT] = std::bind(&FlagRecognizer::extractOrangePresent, this, _1);
	attributeExtractionFunctions[Flag::Attribute::CIRCLES] = std::bind(&FlagRecognizer::extractCircles, this, _1);
	attributeExtractionFunctions[Flag::Attribute::TRIANGLE] = std::bind(&FlagRecognizer::extractTriangle, this, _1);

	loadDataSet(filename);
}

std::string FlagRecognizer::recognize(const cv::Mat& src)
{
	std::array<int, Flag::AttributeCount> srcAttributes;

	for (int i = 0; i < Flag::AttributeCount; i++) {
		if (attributeExtractionFunctions[i]) {
			srcAttributes[i] = attributeExtractionFunctions[i](src);
			std::cout << Flag::getAttributeName(static_cast<Flag::Attribute>(i)) << " : " << srcAttributes[i] << '\n';
		}
	}

	findCountry(srcAttributes);

	return std::string();
}

std::string FlagRecognizer::findCountry(const std::array<int, Flag::AttributeCount>& srcAttributes)
{
	std::map<int, std::string> result;
	for (Flag f : flags) {
		int n = 0;
		for (int i = 0; i < Flag::AttributeCount; i++) {
			if (attributeExtractionFunctions[i]) {
				int attr = f.getAttribute(static_cast<Flag::Attribute>(i));
				if (attr == srcAttributes[i])
					++n;
			}
		}

		result.emplace(n, f.getName());
	}

	for (const auto& it : result) {
		std::cout << it.first << ", " << it.second << '\n';
	}

	return std::string();
}

int FlagRecognizer::extractTriangle(const cv::Mat& src) const
{
	cv::Mat gray;
	cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

	cv::Mat bw;
	cv::Canny(gray, bw, 255, 2500, 5);

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	std::vector<cv::Point> approx;

	for (int i = 0; i < contours.size(); i++) {
		cv::approxPolyDP(cv::Mat(contours[i]), approx,
				cv::arcLength(cv::Mat(contours[i]), true) * 0.02, true);

		// Skip small or non-convex objects
		 if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
			 continue;

		 if (approx.size() == 3)
			 return true;
	}

	return false;
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

	for (size_t i = 0; i < lines.size(); i++) {
		float theta = lines[i][1];

		// TODO: check line size?
		if ((theta < degreesToRadians(10) || theta > degreesToRadians(170)) &&
				lines[i][0] > src.rows / 2)
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

	for (size_t i = 0; i < lines.size(); i++) {
		float theta = lines[i][1];

		// TODO: check line size?
		if (theta > degreesToRadians(80) && theta < degreesToRadians(100) &&
				lines[i][0] > src.cols / 2.5) {
			++n;
		}
	}

	return ((n == 0) ? 0 : (n + 1));
}

int FlagRecognizer::extractColours(const cv::Mat& src) const
{
	cv::Mat hsv;
	cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);

	std::set<int> colorCodeSet;

	int n = 0;
	std::set<uint32_t> colorSet;

	for (Pixel& p : cv::Mat_<Pixel>(hsv)) {
		int colorCode = Colors::getColorCode(p.x, p.y, p.z);
		if (colorCode != -1)
			colorCodeSet.insert(colorCode);
	}

	return colorCodeSet.size();
}

int FlagRecognizer::extractCircles(const cv::Mat& src) const
{
	cv::Mat gray;
	std::vector<cv::Vec3f> circles;
	cvtColor(src, gray, cv::COLOR_BGR2GRAY);
	gray.convertTo(gray, CV_8U);
	cv::GaussianBlur(gray, gray, cv::Size(9, 9), 2, 2);
	cv::HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 2, gray.rows/4, 200, 100);
	return circles.size();
}

bool FlagRecognizer::colorPresent(const cv::Mat& src, Colors::Color color) const
{
	unsigned int n = 0;
	cv::Mat hsv;
	cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);

	for (Pixel& p : cv::Mat_<Pixel>(hsv)) {
		if (Colors::getColorCode(p.x, p.y, p.z) == color)
			++n;
	}

	return (n >= 5 * ((hsv.cols * hsv.rows) / 100));
}

int FlagRecognizer::extractRedPresent(const cv::Mat& src) const
{
	return colorPresent(src, Colors::Color::RED);
}

int FlagRecognizer::extractGreenPresent(const cv::Mat& src) const
{
	return colorPresent(src, Colors::Color::GREEN);
}

int FlagRecognizer::extractBluePresent(const cv::Mat& src) const
{
	return colorPresent(src, Colors::Color::BLUE);
}

int FlagRecognizer::extractGoldPresent(const cv::Mat& src) const
{
	return colorPresent(src, Colors::Color::GOLD);
}

int FlagRecognizer::extractWhitePresent(const cv::Mat& src) const
{
	return colorPresent(src, Colors::Color::WHITE);
}

int FlagRecognizer::extractBlackPresent(const cv::Mat& src) const
{
	return colorPresent(src, Colors::Color::BLACK);
}

int FlagRecognizer::extractOrangePresent(const cv::Mat& src) const
{
	return colorPresent(src, Colors::Color::ORANGE);
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
			else
				flag.setAttribute(attribute, Colors::getColorCode(s));
		}

		flags.push_back(flag);
	}
}

constexpr double FlagRecognizer::degreesToRadians(double angle)
{
	return ((CV_PI / 180) * angle);
}
