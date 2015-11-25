#include <iostream>
#include <fstream>
#include <stdexcept>
#include <opencv2/imgproc/imgproc.hpp>
#include "flagRecognizer.hpp"

using namespace std::placeholders;

FlagRecognizer::FlagRecognizer(const std::string& filename)
{
	attributeExtractionFunctions[Flag::Attribute::BARS] = std::bind(&FlagRecognizer::extractBars, this, _1);

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
	cv::Mat grey;
	cv::cvtColor(src, grey, cv::COLOR_BGR2GRAY);

	cv::Mat edges;
	cv::Canny(grey, edges, 50, 150);

	std::vector<cv::Vec2f> lines;
	cv::HoughLines(edges, lines, 1, CV_PI/180, 120, 0, 0);

	std::cout << lines.size() << '\n';

	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		cv::Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000*(-b));
		pt1.y = cvRound(y0 + 1000*(a));
		pt2.x = cvRound(x0 - 1000*(-b));
		pt2.y = cvRound(y0 - 1000*(a));
		line(src, pt1, pt2, cv::Scalar(0, 255, 0), 3, CV_AA);
	}

	cv::imshow(__func__, src);
	cv::waitKey();

	return 0;
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
