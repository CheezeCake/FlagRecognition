#include <iostream>
#include <opencv/cv.hpp>
#include "flagRecognizer.hpp"

int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cerr << "usage : " << argv[0] << " flagImg\n";
		return 1;
	}

	cv::Mat flag = cv::imread(argv[1]);
	FlagRecognizer recognizer("flag.data");

	recognizer.recognize(flag);

	return 0;
}
