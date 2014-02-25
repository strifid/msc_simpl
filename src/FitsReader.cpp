/*
 * FitsReader.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: trifid
 */

#include "FitsReader.h"
#include "CCfits"
#include <iterator>

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
	for (long j = 0; j < m_y; j += 10) {
		std::ostream_iterator<short> c(std::cout, "\t");
		std::copy(&contents[j * m_x], &contents[(j + 1) * m_x - 1], c);
		std::cout << '\n';
	}

	std::cout << std::endl;

}

FitsReader::~FitsReader() {
}

