/*
 * FitsReader.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: trifid
 */

#include "FitsReader.h"
#include "CCfits"
#include <iterator>

#include "opencv2/core/core.hpp"
#include "Pixel.h"
using cv::Scalar;

using namespace CCfits;

FitsReader::FitsReader() {
	m_fits = NULL;
}

bool FitsReader::init(const std::string& path) {

	m_fits = new FITS(path.c_str(), Read, true);

	PHDU& image = m_fits->pHDU();

	// read all user-specifed, coordinate, and checksum keys in the image
	image.readAllKeys();
	image.read<unsigned long>(contents);

	// this doesn't print the data, just header info.
	std::cout << image << std::endl;

	m_x = image.axis(0);
	m_y = image.axis(1);

	return true;
}

void FitsReader::fillMat(cv::Mat& m) {

	Mat fitsMat(m_y, m_x, CV_32S, Scalar(35555, 35555, 35555));

	for (int i = 0; i < m_x; i++) {
		for (int j = 0; j < m_y; j++) {
			fitsMat.at<int32_t>(Pixel(i, j)) = contents[i * m_x + j];
		}
	}
	std::cout << std::endl;

	m = fitsMat;
}

FitsReader::~FitsReader() {
}

