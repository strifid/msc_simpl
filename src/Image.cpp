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

void Image::mirrorEdges(Mat& img) {

	Mat mirrorEdgesMat = Mat(img.rows + 2, img.cols + 2, img.type());
	mirrorEdgesMat.setTo(Scalar(0, 0, 0));

	img.copyTo(mirrorEdgesMat(cv::Rect(Point(1, 1), img.size())));
	mirrorEdgesMat.col(2).copyTo(mirrorEdgesMat(cv::Rect(Point(0, 0), mirrorEdgesMat.col(2).size())));
	mirrorEdgesMat.col(mirrorEdgesMat.cols - 3).copyTo(
			mirrorEdgesMat(cv::Rect(Point(mirrorEdgesMat.cols - 1, 0), mirrorEdgesMat.col(mirrorEdgesMat.cols - 3).size())));

	mirrorEdgesMat.row(2).copyTo(mirrorEdgesMat(cv::Rect(Point(0, 0), mirrorEdgesMat.row(2).size())));
	mirrorEdgesMat.row(mirrorEdgesMat.rows - 3).copyTo(
			mirrorEdgesMat(cv::Rect(Point(0, mirrorEdgesMat.rows - 1), mirrorEdgesMat.row(mirrorEdgesMat.rows - 3).size())));
	img = mirrorEdgesMat;
}

bool Image::init(const std::string & path) {
	m_imagePath = path;

	m_img = imread(path.c_str(), 0);
//	mirrorEdges(m_img);
	m_forChanges = imread(path.c_str(), 1);
	m_width = m_img.cols;
	m_height = m_img.rows;

	return false;
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

Pixels Image::getOneConnectedAround(int x, int y) {
	//todo move in members and return link
	Pixels pts;

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

//TTTTTTTTTTTTTTT
int32_t Image::value(const Pixel & pt) {
	return m_img.at<uint8_t>(dynamic_cast<const Point&>(pt));
}

void Image::setValue(const Pixel & pt, int32_t val) {
	m_img.at<uint8_t>(pt) = val;
}

bool Image::isBoundaryPixel(const Pixel & pt) {
	if (pt.x == 0 || pt.x == m_width - 1)
		return true;
	if (pt.y == 0 || pt.y == m_height - 1)
		return true;

	return false;
}

int32_t Image::comparePixels(const Pixel& a, const Pixel& b) {
	int res = ::abs(value(a)) - ::abs(value(b));

	if (abs(res) <= m_sensivity)
		return 0;

	if (value(a) < value(b))
		return -1;

	return 1;
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

void Image::paintPixel(Vertex *pxl, BrushColor color) {
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

