/*
 * DiscreteMorsePixel.cpp
 *
 *  Created on: Jul 8, 2012
 *      Author: trifid
 */

#include "Pixel.h"
#include "Utils.h"
Pixel::Pixel() {
}

Pixel::Pixel(int int1, int int2) :
		Point(int1, int2) {
}

Pixel::~Pixel() {
}


std::string Pixel::toString() const {
	std::string str;
	str = "(x:" + Utils::intToString(x) + " y:" + Utils::intToString(y) + ") ";
	return str;
}

