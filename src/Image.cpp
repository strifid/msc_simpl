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

uint32_t Image::m_width = 0;
uint32_t Image::m_height = 0;

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

	Mat torMat = Mat(img.rows * 2 - 2, img.cols * 2 - 2, img.type());

	torMat.setTo(Scalar(0, 0, 0));

	for (int x = 0; x < img.rows* 2 - 2 ; x++) {
		for (int y = 0; y < img.cols* 2 - 2; y++) {


			int origX = (x >= img.rows) ? 2 * img.rows - x - 2 : x;
			int origY = (y >= img.cols) ? 2 * img.cols - y - 2 : y;

			torMat.at<uint8_t>(x, y) = img.at<uint8_t>(origX, origY);
		}
	}
	img = torMat;
}

bool Image::init(const std::string & path) {
	m_imagePath = path;

	m_img = imread(path.c_str(), 0);
	mirrorEdges(m_img);
	m_forChanges = imread(path.c_str(), 1);
	m_width = m_img.cols;
	m_height = m_img.rows;

	return false;
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

