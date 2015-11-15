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

uint32_t Image::m_originalWidth = 0;
uint32_t Image::m_originalHeight = 0;

Image::Image() :
		m_imageName("Image") {
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

	for (int x = 0; x < img.rows * 2 - 2; x++) {
		for (int y = 0; y < img.cols * 2 - 2; y++) {

			int origX = (x >= img.rows) ? 2 * img.rows - x - 2 : x;
			int origY = (y >= img.cols) ? 2 * img.cols - y - 2 : y;

			torMat.at<uint8_t>(x, y) = img.at<uint8_t>(origX, origY);
		}
	}
	img = torMat;
}

using namespace cv;

bool Image::init(const std::string & path) {
	m_imagePath = path;

	m_img = imread(path.c_str(), 0);
	m_originalWidth = m_img.cols;
	m_originalHeight = m_img.rows;

	mirrorEdges(m_img);
	Mat img = imread(path.c_str(), 1);
	{

		Mat torMat = Mat(img.rows * 2 - 2, img.cols * 2 - 2, img.type());

		torMat.setTo(Scalar(0, 0, 0));

		for (int x = 0; x < img.rows * 2 - 2; x++) {
			for (int y = 0; y < img.cols * 2 - 2; y++) {

				int origX = (x >= img.rows) ? 2 * img.rows - x - 2 : x;
				int origY = (y >= img.cols) ? 2 * img.cols - y - 2 : y;

				torMat.at<Vec3b>(x, y) = img.at<Vec3b>(origX, origY);
			}
		}
		m_forChanges = torMat;

	}
	m_width = m_img.cols;
	m_height = m_img.rows;

	return false;
}

void Image::saveAs(const std::string& path, bool show) {

	paint(m_forChanges);

	cv::Rect roi(0, 0, (m_forChanges.cols + 2) / 2, (m_forChanges.rows + 2) / 2);
	cv::Mat orig;
	cv::Mat(m_forChanges, roi).copyTo(orig);

	if (show) {
		imshow(path.c_str(), orig);
		waitKey(0);
	}

	imwrite(path.c_str(), orig);
}

void Image::drawCircle(VertexPtr vtx, cv::Scalar color, uint32_t radius, uint32_t thickness) {
	circle(m_forChanges, Point(vtx->x, vtx->y), radius, color, thickness);
}

//TTTTTTTTTTTTTTT
int32_t Image::value(uint32_t x, uint32_t y) {
	return m_img.at<uint8_t>(Point(x, y));
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

bool Image::isOut(VertexPtr vtx) {

	if ((vtx->x > m_width * 0.65 && vtx->x < m_width * 0.85) || (vtx->y > m_height * 0.65 && vtx->y < m_height * 0.85))
		return true;

	return false;
}
