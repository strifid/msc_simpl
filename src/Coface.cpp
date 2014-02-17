/*
 * Coface.cpp
 *
 *  Created on: Dec 4, 2012
 *      Author: trifid
 */

#include "Coface.h"
#include "opencv2/core/core.hpp"
#include "opencv2/core/core_c.h"

using cv::line;
using cv::Scalar;

Coface::Coface() {
	// TODO Auto-generated constructor stub

}

Coface::~Coface() {
	// TODO Auto-generated destructor stub
}

void Coface::drawArrow(Mat & img, Point & a, Point & b) {

	int thickness = 2;
	int lineType = 8;

	line(img, a, b, Scalar(255, 0, 0), thickness, lineType);

	double angle = std::atan2((double) a.y - b.y, (double) a.x - b.x);

	Point p;
	p.x = (int) (b.x + (10 * cos(angle + 3.1416 / 6)));
	p.y = (int) (b.y + (10 * sin(angle + 3.1416 / 6)));
	line(img, p, b, Scalar(255, 0, 0), 2, CV_AA, 0);

	p.x = (int) (b.x + 10 * cos(angle - 3.1416 / 6));
	p.y = (int) (b.y + 10 * sin(angle - 3.1416 / 6));
	line(img, p, b, Scalar(255, 0, 0), 2, CV_AA, 0);

}

