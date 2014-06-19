/*
 * generate_image.cpp
 *
 *  Created on: Jun 16, 2014
 *      Author: trifid
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>

#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iosfwd>

using cv::Mat;
using cv::imread;
using cv::namedWindow;
using cv::waitKey;
using cv::Scalar;

int main() {

	{
		const int x = 6;
		const int y = 4;

		uint8_t buf[y][x] = { { 24, 12, 35, 69, 72, 56 }, { 1, 2, 2, 21, 32, 23 }, { 2, 0, 0, 0, 5, 12 }, { 47, 24, 3, 0, 8, 34 } };
		Mat img(y, x, CV_8UC1, Scalar(255, 255, 255));

		for (size_t i = 0; i < y; i++) {
			for (size_t j = 0; j < x; j++) {
				img.at<uint8_t>(i, j) = buf[i][j];
			}
		}

		imwrite("test.bmp", img);
		imshow("test.bmp", img);
		waitKey(0);
		return 0;

	}

}
