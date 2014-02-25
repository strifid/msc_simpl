/*
 * Image.cpp
 *
 *  Created on: Jul 7, 2012
 *      Author: trifid
 */

#include "Image.h"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include "opencv2/core/core.hpp"
#include <iosfwd>

#include "FitsReader.h"

using cv::imread;
using cv::namedWindow;
using cv::waitKey;
using cv::imwrite;
using cv::Scalar;
using cv::circle;

Image::Image() :
		m_sensivity(0), m_imageName("Image") {
	MT_LOGINIT(Image);
	namedWindow(m_imageName.c_str());

}

Image::~Image() {
}

#include <iostream>
#include <fstream>

bool Image::initFits(const std::string & path) {

	m_imagePath = path;

	FitsReader fr;
	fr.init(path);

	fr.fillMat(m_img);
	m_width = fr.getX();
	m_height = fr.getY();

	return false;
}

bool Image::init(const std::string & path) {
	m_imagePath = path;
	m_img = imread(path.c_str(), 0);
	m_forChanges = imread(path.c_str(), 1);
	m_width = m_img.cols;
	m_height = m_img.rows;

	return false;
}

/*
 bool Image::hasWrapNull(const Pixel& pt) {

 Pixels pts = getAllAround(pt);
 for (size_t i = 0; i < pts.size(); i++) {
 if (!comparePixels(pt, pts[i]))
 return true;
 }
 return false;
 }

 */
void Image::mirrorEdges() {

	Mat mirroreImg;
	cv::hconcat(m_img.row(0), m_img, mirroreImg);
//	cv::hconcat(m_img.row(0), m_img, mirroreImg);

}

Pixels Image::getAllAround(const Pixel& pt) {
	//todo move in members and return link
	Pixels pts;

	uint32_t x = pt.x;
	uint32_t y = pt.y;

	if (x == 0) {
		pts.push_back(Pixel(x + 1, y));
		if (y == 0) {
			pts.push_back(Pixel(x, y + 1));
			pts.push_back(Pixel(x + 1, y + 1));
		} else if (y == m_height - 1) {
			pts.push_back(Pixel(x, y - 1));
			pts.push_back(Pixel(x + 1, y - 1));
		} else {
			pts.push_back(Pixel(x, y + 1));
			pts.push_back(Pixel(x + 1, y + 1));
			pts.push_back(Pixel(x, y - 1));
			pts.push_back(Pixel(x + 1, y - 1));
		}
	} else if (x == m_width - 1) {
		pts.push_back(Pixel(x - 1, y));
		if (y == 0) {
			pts.push_back(Pixel(x, y + 1));
			pts.push_back(Pixel(x - 1, y + 1));
		} else if (y == m_height - 1) {
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

		} else if (y == m_height - 1) {
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

Pixels Image::getOneConnectedAround(const Pixel& pt) {
	//todo move in members and return link
	Pixels pts;

	uint32_t x = pt.x;
	uint32_t y = pt.y;

	if (x == 0) {
		pts.push_back(Pixel(x + 1, y));
		if (y == 0) {
			pts.push_back(Pixel(x, y + 1));
		} else if (y == m_height - 1) {
			pts.push_back(Pixel(x, y - 1));
		} else {
			pts.push_back(Pixel(x, y + 1));
			pts.push_back(Pixel(x, y - 1));
		}
	} else if (x == m_width - 1) {
		pts.push_back(Pixel(x - 1, y));
		if (y == 0) {
			pts.push_back(Pixel(x, y + 1));
		} else if (y == m_height - 1) {
			pts.push_back(Pixel(x, y - 1));
		} else {
			pts.push_back(Pixel(x, y + 1));
			pts.push_back(Pixel(x, y - 1));
		}
	} else {
		pts.push_back(Pixel(x - 1, y));
		pts.push_back(Pixel(x + 1, y));

		if (y == 0) {
			pts.push_back(Pixel(x, y + 1));
		} else if (y == m_height - 1) {
			pts.push_back(Pixel(x, y - 1));
		} else {
			pts.push_back(Pixel(x, y - 1));
			pts.push_back(Pixel(x, y + 1));
		}
	}

	return pts;
}

void Image::saveAs(const std::string& path, bool show) {

	paint(m_forChanges);
	if (show) {
		imshow(path.c_str(), m_forChanges);
		waitKey(0);
	}

	imwrite(path.c_str(), m_forChanges);
}

void Image::drawCircle(Point point, cv::Scalar color, uint32_t radius, uint32_t thickness) {
	circle(m_forChanges, point, radius, color, thickness);
}

uint8_t Image::value(const Pixel & pt) {
	return m_img.at<uint8_t>(dynamic_cast<const Point&>(pt));
}

void Image::setValue(const Pixel & pt, uint8_t val) {
	m_img.at<uint8_t>(pt) = val;
}

/*
 bool Image::process() {
 return false;
 }
 */

bool Image::isBoundaryPixel(const Pixel & pt) {
	if (pt.x == 0 || pt.x == m_width - 1)
		return true;
	if (pt.y == 0 || pt.y == m_height - 1)
		return true;

	return false;
}

void Image::findSubset(Subset & subset) {
	bool addPoint = false;

	do {
		addPoint = false;
		Pixels pts = subset.getContour();

		for (size_t i = 0; i < pts.size(); i++) {
			Pixels contourAroundPixels = getAllAround(pts[i]);
			for (size_t j = 0; j < contourAroundPixels.size(); j++) {
				if (!comparePixels(contourAroundPixels[j], subset.center()) && !subset.isInSubset(contourAroundPixels[j])) {
					subset.addPoint(contourAroundPixels[j]);
					addPoint = true;
				}
			}

		}
	} while (addPoint);

}

int32_t Image::comparePixels(const Pixel& a, const Pixel& b) {
	int res = ::abs(value(a)) - ::abs(value(b));

	if (abs(res) <= m_sensivity)
		return 0;

	if (value(a) < value(b))
		return -1;

	return 1;
}

void Image::locateSubsetAndAllAround(Subset& subset) {

	bool addPoint = false;
	while (subset.wrap(Wrap::O).points().size()) {

		Wrap wrap0;
		for (PointsMap::iterator it = subset.wrap(Wrap::O).points().begin(); it != subset.wrap(Wrap::O).points().end(); it++) {
			subset.addPoint(it->first);
			subset.wrap(Wrap::BD).points().erase(it->first);

			Pixels contourAroundPixels = getAllAround(it->first);

			for (size_t j = 0; j < contourAroundPixels.size(); j++) {
				if (!subset.isInSubsetAndWraps(contourAroundPixels[j])) {

					switch (comparePixels(contourAroundPixels[j], subset.center())) {
					case 0:
						wrap0.addPoint(contourAroundPixels[j]);
						break;
					case 1:
						subset.wrap(Wrap::UP_WRAP).addPoint(contourAroundPixels[j]);
						break;
					case -1:
						subset.wrap(Wrap::DOWN).addPoint(contourAroundPixels[j]);
						break;
					}
					subset.wrap(Wrap::BD).addPoint(contourAroundPixels[j]);
				}
			}
		}
		subset.wrap(Wrap::O) = wrap0;

	}
}

void Image::resetPainting() {

	m_greenPxls.clear();
	m_redPxls.clear();
	m_bluePxls.clear();

}

void Image::paint(Mat& forChanges) {

	for (size_t i = 0; i < m_greenPxls.size(); i++) {
		forChanges.at<cv::Vec3b>(m_greenPxls[i].y, m_greenPxls[i].x)[0] = 0x00;
		forChanges.at<cv::Vec3b>(m_greenPxls[i].y, m_greenPxls[i].x)[1] = 0xFF;
		forChanges.at<cv::Vec3b>(m_greenPxls[i].y, m_greenPxls[i].x)[2] = 0x00;
	}

	for (size_t i = 0; i < m_redPxls.size(); i++) {
		forChanges.at<cv::Vec3b>(m_redPxls[i].y, m_redPxls[i].x)[0] = 0x00;
		forChanges.at<cv::Vec3b>(m_redPxls[i].y, m_redPxls[i].x)[1] = 0x00;
		forChanges.at<cv::Vec3b>(m_redPxls[i].y, m_redPxls[i].x)[2] = 0xFF;
	}

	for (size_t i = 0; i < m_bluePxls.size(); i++) {
		forChanges.at<cv::Vec3b>(m_bluePxls[i].y, m_bluePxls[i].x)[0] = 0xFF;
		forChanges.at<cv::Vec3b>(m_bluePxls[i].y, m_bluePxls[i].x)[1] = 0x00;
		forChanges.at<cv::Vec3b>(m_bluePxls[i].y, m_bluePxls[i].x)[2] = 0x00;
	}

}

void Image::paintPixel(Pixel *pxl, BrushColor color) {
	switch (color) {
	case RED:
		m_forChanges.at<cv::Vec3b>(pxl->y, pxl->x)[0] = 0x00;
		m_forChanges.at<cv::Vec3b>(pxl->y, pxl->x)[1] = 0x00;
		m_forChanges.at<cv::Vec3b>(pxl->y, pxl->x)[2] = 0xFF;
		break;

	case GREEN:
		m_forChanges.at<cv::Vec3b>(pxl->y, pxl->x)[0] = 0x00;
		m_forChanges.at<cv::Vec3b>(pxl->y, pxl->x)[1] = 0xFF;
		m_forChanges.at<cv::Vec3b>(pxl->y, pxl->x)[2] = 0x00;
		break;

	case BLUE:
		m_forChanges.at<cv::Vec3b>(pxl->y, pxl->x)[0] = 0xFF;
		m_forChanges.at<cv::Vec3b>(pxl->y, pxl->x)[1] = 0x00;
		m_forChanges.at<cv::Vec3b>(pxl->y, pxl->x)[2] = 0x00;
		break;
	}
}

