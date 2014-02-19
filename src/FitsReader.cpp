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

	m_fits = new FITS("atestfil.fit", Read, true);

	PHDU& image = m_fits->pHDU();

	std::valarray<unsigned long> contents;

	// read all user-specifed, coordinate, and checksum keys in the image
	image.readAllKeys();
	image.read<unsigned long>(contents);

	// this doesn't print the data, just header info.
	std::cout << image << std::endl;

	long ax1(image.axis(0));
	long ax2(image.axis(1));

	for (long j = 0; j < ax2; j += 10) {
		std::ostream_iterator<short> c(std::cout, "\t");
		std::copy(&contents[j * ax1], &contents[(j + 1) * ax1 - 1], c);
		std::cout << '\n';
	}

	std::cout << std::endl;
	return true;
}

FitsReader::~FitsReader() {
}

