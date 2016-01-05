#pragma once

#include <vector>
#include <array>
#include <functional>
#include <opencv/cv.hpp>
#include "flag.hpp"
#include "colors.hpp"

class FlagRecognizer
{
	public:
		explicit FlagRecognizer(const std::string& filename);
		std::string recognize(const cv::Mat& src);
		std::string findCountry(const std::array<int, Flag::AttributeCount>& srcAttributes);

		int extractBars(const cv::Mat& src) const;
		int extractStripes(const cv::Mat& src) const;
		int extractColours(const cv::Mat& src) const;
		int extractRedPresent(const cv::Mat& src) const;
		int extractGreenPresent(const cv::Mat& src) const;
		int extractBluePresent(const cv::Mat& src) const;
		int extractGoldPresent(const cv::Mat& src) const;
		int extractWhitePresent(const cv::Mat& src) const;
		int extractBlackPresent(const cv::Mat& src) const;
		int extractOrangePresent(const cv::Mat& src) const;
		int extractCircles(const cv::Mat& src) const;

	private:
		static constexpr double degreesToRadians(double angle);
		void loadDataSet(const std::string& filename);
		bool colorPresent(const cv::Mat& src, Colors::Color color) const;

		std::vector<Flag> flags;

		std::array<std::function<int(const cv::Mat&)>, Flag::AttributeCount> attributeExtractionFunctions;
};
