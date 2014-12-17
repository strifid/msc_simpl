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
#include "Utils.h"

using cv::putText;
using cv::Scalar;

Vertex::~Vertex() {
}

Vertex::Vertex(int xx, int yy) {
	x = xx;
	y = yy;
}

Vertex::Vertex(int xx, int yy, int val) {
	x = xx;
	y = yy;
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

	img.at<cv::Vec3b>(y * Image::m_enlargeFactor, x * Image::m_enlargeFactor)[0] = value().getInt();

	putText(img, Utils::intToString(value().getInt()),
			Point(x * Image::m_enlargeFactor + Image::m_enlargeFactor, y * Image::m_enlargeFactor + Image::m_enlargeFactor), CV_FONT_NORMAL, 0.7,
			color, 1.3);

}

bool Vertex::operator !=(const Vertex& vrtx) const {
	return !operator==(vrtx);
}


std::ostream & operator <<(std::ostream & out, const Vertex& vt) {

	out << "x: " << vt.x << ", y: " << vt.y << " ";
	return out;

}
