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
		uint8_t buf[9][15] = { { 50, 20, 50, 80, 100, 80, 50, 20, 50, 80, 110, 90, 60, 40, 60 }, { 10, 0, 10, 20, 30, 20, 10, 1, 15, 30, 40, 33, 25,
				20, 25 }, { 50, 20, 50, 80, 100, 80, 50, 20, 50, 80, 110, 90, 60, 40, 60 }, { 80, 40, 80, 130, 180, 140, 90, 40, 80, 130, 180, 130,
				90, 60, 90 }, { 120, 60, 120, 180, 250, 180, 120, 50, 110, 170, 240, 190, 140, 90, 140 }, { 80, 40, 80, 130, 180, 140, 90, 40, 80,
				130, 180, 130, 90, 60, 90 }, { 50, 20, 50, 80, 100, 80, 50, 20, 50, 80, 110, 90, 60, 40, 60 }, { 10, 0, 10, 20, 30, 20, 10, 1, 15, 30,
				40, 33, 25, 20, 25 }, { 50, 20, 50, 80, 100, 80, 50, 20, 50, 80, 110, 90, 60, 40, 60 } };

		Mat img(9, 15, CV_8UC1, Scalar(255, 255, 255));

		for (size_t i = 0; i < 9; i++) {
			for (size_t j = 0; j < 15; j++) {
				img.at<uint8_t>(i, j) = buf[i][j];
			}
		}

		imwrite("test.bmp", img);
		imshow("test.bmp", img);
		waitKey(0);
		return 0;

	}

}
