#pragma once

#include <vector>
#include <array>
#include <functional>
#include <opencv/cv.hpp>
#include "flag.hpp"

class FlagRecognizer
{
	public:
		explicit FlagRecognizer(const std::string& filename);
		std::string recognize(const cv::Mat& src);

		int extractBars(const cv::Mat& src) const;
		int extractStripes(const cv::Mat& src) const;
		int extractColours(const cv::Mat& src) const;

	private:
		static constexpr double degreesToRadians(double angle);
		void loadDataSet(const std::string& filename);
		std::vector<Flag> flags;

		std::array<std::function<int(const cv::Mat&)>, Flag::AttributeCount> attributeExtractionFunctions;
};
