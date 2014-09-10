/*
 * FitsReader.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: trifid
 */

#include "FitsReader.h"
#include "CCfits/CCfits"
#include <cmath>
#include <iterator>

#include "opencv2/core/core.hpp"
#include "Pixel.h"
using cv::Scalar;

using namespace CCfits;

FitsReader::FitsReader() {
	m_fits = NULL;
	m_x = 0;
	m_y = 0;
}

bool FitsReader::init(const std::string& path) {

	m_fits = new FITS(path.c_str(), Read, true);

	std::cout << "\nCurrent extension: " << m_fits->currentExtensionName() << std::endl;

	ExtHDU& image = m_fits->currentExtension();
	 image.readAllKeys();
	 image.read<unsigned long>(contents);

	 // this doesn't print the data, just header info.
	 std::cout << image << std::endl;

	 m_x = image.axis(0);
	 m_y = image.axis(1);

//	exit(0);
	/*
	 PHDU& image = m_fits->pHDU();

	 // read all user-specifed, coordinate, and checksum keys in the image
	 image.readAllKeys();
	 image.read<unsigned long>(contents);

	 // this doesn't print the data, just header info.
	 std::cout << image << std::endl;

	 m_x = image.axis(0);
	 m_y = image.axis(1);
	 */

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

