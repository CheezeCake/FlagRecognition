#include <iostream>
#include <opencv/cv.hpp>
#include "flagRecognizer.hpp"
#include "colors.hpp"

int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cerr << "usage : " << argv[0] << " flagImg\n";
		return 1;
	}

	try {
		cv::Mat flag = cv::imread(argv[1]);
		FlagRecognizer recognizer("flag.data");

		recognizer.recognize(flag);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return 2;
	}
	catch (...) {
		return 3;
	}

	return 0;
}
