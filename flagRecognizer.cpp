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
	attributeExtractionFunctions[Flag::Attribute::CRESCENT] = std::bind(&FlagRecognizer::extractCrescent, this, _1);

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

	if (!result.empty()) {
		std::cout << "\nmatching attributes | country\n";
		auto& it = --result.cend();
		do {
			std::cout << it->first << " | " << it->second << '\n';
		} while (it-- != result.cbegin());

		return result.cbegin()->second;
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

		if ((theta < degreesToRadians(10) || theta > degreesToRadians(170)))
			//&& lines[i][0] > src.rows / 2)
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

		if (theta > degreesToRadians(80) && theta < degreesToRadians(100))
			//&& lines[i][0] > src.cols / 2.5)
			++n;
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

	unsigned int count = 0;

	for(cv::Vec3f circle : circles)
		if(isCircleOrCrescent(circle, src, true))
			count++;

	return count;
}

int FlagRecognizer::extractCrescent(const cv::Mat& src) const
{
	cv::Mat gray;
	std::vector<cv::Vec3f> circles;

	cvtColor(src, gray, cv::COLOR_BGR2GRAY);

	gray.convertTo(gray, CV_8U);
	cv::GaussianBlur(gray, gray, cv::Size(9, 9), 2, 2);

	cv::HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 2, gray.rows/4, 200, 100);

	for(cv::Vec3f circle : circles)
		if(isCircleOrCrescent(circle, src, false))
			return 1;
	return 0;
}

bool FlagRecognizer::isCircleOrCrescent(cv::Vec3f circle, const cv::Mat& src, bool circleNotCrescent) const
{
	const int ORIGIN_X = 0;
	const int ORIGIN_Y = 1;
	const int RADIUS = 2;

	cv::Mat hsv;
	cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);

	cv::Mat hsv_channels[3];
	cv::split(hsv, hsv_channels);

	/*cv::Mat rotated;
	cv::Mat rotationMat;
	cv::Point2f origin(circle[ORIGIN_X], circle[ORIGIN_Y]);
	rotationMat = cv::getRotationMatrix2D(origin, 35, 1.0);
	cv::warpAffine(hsv_channels[2], rotated, rotationMat, cv::Size(src.cols, src.rows));*/

	Pixel top = hsv_channels[2].at<Pixel>(circle[ORIGIN_X], circle[ORIGIN_Y] - circle[RADIUS]);
	Pixel bottom = hsv_channels[2].at<Pixel>(circle[ORIGIN_X], circle[ORIGIN_Y] + circle[RADIUS]);
	Pixel left = hsv_channels[2].at<Pixel>(circle[ORIGIN_X] - circle[RADIUS], circle[ORIGIN_Y]);
	Pixel right = hsv_channels[2].at<Pixel>(circle[ORIGIN_X] + circle[RADIUS], circle[ORIGIN_Y]);

	/*Pixel top2 = rotated.at<Pixel>(circle[ORIGIN_X], circle[ORIGIN_Y] - circle[RADIUS]);
	Pixel bottom2 = rotated.at<Pixel>(circle[ORIGIN_X], circle[ORIGIN_Y] + circle[RADIUS]);
	Pixel left2 = rotated.at<Pixel>(circle[ORIGIN_X] - circle[RADIUS], circle[ORIGIN_Y]);
	Pixel right2 = rotated.at<Pixel>(circle[ORIGIN_X] + circle[RADIUS], circle[ORIGIN_Y]);*/

	int top_color = Colors::getColorCode(top.x, top.y, top.z);
	int bottom_color = Colors::getColorCode(bottom.x, bottom.y, bottom.z);
	int left_color = Colors::getColorCode(left.x, left.y, left.z);
	int right_color = Colors::getColorCode(right.x, right.y, right.z);

	/*int top_color2 = Colors::getColorCode(top2.x, top2.y, top2.z);
	int bottom_color2 = Colors::getColorCode(bottom2.x, bottom2.y, bottom2.z);
	int left_color2 = Colors::getColorCode(left2.x, left2.y, left2.z);
	int right_color2 = Colors::getColorCode(right2.x, right2.y, right2.z);*/


	/*printf("TOP : %d\n", top_color);
	printf("BOTTOM: %d\n", bottom_color);
	printf("LEFT : %d\n", left_color);
	printf("RIGHT : %d\n", right_color);*/

	/*bool originalCircle = false;
	bool rotatedCircle = false;
	bool originalCrescent = false;
	bool rotatedCrescent = false;*/

	if(circleNotCrescent && top_color == bottom_color && left_color == right_color && top_color == left_color)
	//if(top_color == bottom_color && left_color == right_color && top_color == left_color)
	//	originalCircle = true;
		return true;

	//else if( 
	else if(!circleNotCrescent && 
			((top_color == bottom_color && top_color == left_color && top_color != right_color)
			 || (top_color != bottom_color && top_color == left_color && top_color == right_color) 
			 || (top_color == bottom_color && top_color != left_color && top_color == right_color)
			 || (top_color != bottom_color && bottom_color == left_color && bottom_color != right_color)))
		//originalCrescent = true;
		return true;

	/*if(top_color2 == bottom_color2 && left_color2 == right_color2 && top_color2 == left_color2)
		rotatedCircle = true;

	else if( 
			((top_color2 == bottom_color2 && top_color2 == left_color2 && top_color2 != right_color2)
			 || (top_color2 != bottom_color2 && top_color2 == left_color2 && top_color2 == right_color2) 
			 || (top_color2 == bottom_color2 && top_color2 != left_color2 && top_color2 == right_color2)
			 || (top_color2 != bottom_color2 && bottom_color2 == left_color2 && bottom_color2 != right_color2)))
		rotatedCrescent = true;*/
		
	/*if(circleNotCrescent && originalCircle && rotatedCircle)
		return true;

	if(!circleNotCrescent && ((originalCrescent && rotatedCrescent)
			|| (originalCrescent && rotatedCircle)
			|| (originalCircle && rotatedCrescent)))
		return true;*/

	return false;
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
