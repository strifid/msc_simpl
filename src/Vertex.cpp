/*
 * Vertex.cpp
 *
 *  Created on: Nov 15, 2012
 *      Author: trifid
 */

#include "Vertex.h"
#include "Image.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "mt/utils/StrUtils.h"

using cv::putText;
using cv::Scalar;

Vertex::~Vertex() {
}

Vertex::Vertex(int xx, int yy) {
	x = xx;
	y = yy;
	m_value = 0;
}

Vertex::Vertex(int xx, int yy, int val) {
	x = xx;
	y = yy;
	m_value = val;

}

uint32_t Vertex::locateSubset(Image& img) {

	bool addPointFlag = false;

	do {
		addPointFlag = false;
		Pixels pts = getContour();

		for (size_t i = 0; i < pts.size(); i++) {
			Pixels contourAroundPixels = img.getAllAround(pts[i]);
			for (size_t j = 0; j < contourAroundPixels.size(); j++) {
				if (!img.comparePixels(contourAroundPixels[j], center()) && !isInSubset(contourAroundPixels[j])) {
					addPoint(contourAroundPixels[j]);
					addPointFlag = true;
				}
			}
		}
	} while (addPointFlag);

	return 0;
}

bool Vertex::operator ==(const Vertex& vrtx) const {
	if (x == vrtx.x)
		return y == vrtx.y;
	return false;
}

bool Vertex::operator <(const Vertex& vrtx) const {
	if (x == vrtx.x)
		return y < vrtx.y;
	return x < vrtx.x;
}

void Vertex::draw(Mat& img, Scalar color) {

	img.at<cv::Vec3b>(y * Image::m_enlargeFactor, x * Image::m_enlargeFactor)[0] = m_value;
	putText(img, mt::StrUtils::intToString(value()),
			Point(x * Image::m_enlargeFactor + Image::m_enlargeFactor, y * Image::m_enlargeFactor + Image::m_enlargeFactor), CV_FONT_NORMAL, 0.7,
			color, 1.3);
}

Pixels Vertex::getAllAround(const Pixel& pt, uint32_t width, uint32_t height) {
	//todo move in members and return link
	Pixels pts;

	uint32_t x = pt.x;
	uint32_t y = pt.y;

	if (x == 0) {
		pts.push_back(Pixel(x + 1, y));
		if (y == 0) {
			pts.push_back(Pixel(x, y + 1));
			pts.push_back(Pixel(x + 1, y + 1));
		} else if (y == height - 1) {
			pts.push_back(Pixel(x, y - 1));
			pts.push_back(Pixel(x + 1, y - 1));
		} else {
			pts.push_back(Pixel(x, y + 1));
			pts.push_back(Pixel(x + 1, y + 1));
			pts.push_back(Pixel(x, y - 1));
			pts.push_back(Pixel(x + 1, y - 1));
		}
	} else if (x == width - 1) {
		pts.push_back(Pixel(x - 1, y));
		if (y == 0) {
			pts.push_back(Pixel(x, y + 1));
			pts.push_back(Pixel(x - 1, y + 1));
		} else if (y == height - 1) {
			pts.push_back(Pixel(x, y - 1));
			pts.push_back(Pixel(x - 1, y - 1));
		} else {
			pts.push_back(Pixel(x, y + 1));
			pts.push_back(Pixel(x - 1, y + 1));
			pts.push_back(Pixel(x, y - 1));
			pts.push_back(Pixel(x - 1, y - 1));
		}
	} else {
		pts.push_back(Pixel(x - 1, y));
		pts.push_back(Pixel(x + 1, y));

		if (y == 0) {
			pts.push_back(Pixel(x - 1, y + 1));
			pts.push_back(Pixel(x, y + 1));
			pts.push_back(Pixel(x + 1, y + 1));

		} else if (y == height - 1) {
			pts.push_back(Pixel(x - 1, y - 1));
			pts.push_back(Pixel(x, y - 1));
			pts.push_back(Pixel(x + 1, y - 1));
		} else {
			pts.push_back(Pixel(x - 1, y - 1));
			pts.push_back(Pixel(x, y - 1));
			pts.push_back(Pixel(x + 1, y - 1));
			pts.push_back(Pixel(x - 1, y + 1));
			pts.push_back(Pixel(x, y + 1));
			pts.push_back(Pixel(x + 1, y + 1));

		}

	}

	return pts;
}

std::ostream & operator <<(std::ostream & out, const Vertex& vt) {

	out << "x: " << vt.x << ", y: " << vt.y << " ";
	return out;

}

void Vertex::value(int32_t i) {

//	std::cout << "set vertex " << i << std::endl;

	m_value = i;
	m_valueFirst = i;

}

