/*
 * FitsReader.h
 *
 *  Created on: Feb 17, 2014
 *      Author: trifid
 */

#ifndef FITSREADER_H_
#define FITSREADER_H_

//#include "CCfits/FITS.h"
#include <string>
#include <valarray>
#include "opencv2/core/core.hpp"

using cv::Mat;

namespace CCfits {
class FITS;
} /* namespace CCfits */

class FitsReader {
public:

	void fillMat(cv::Mat& m);

	bool init(const std::string& path);
	FitsReader();
	virtual ~FitsReader();

	long getX() const {
		return m_x;
	}

	void setX(long x) {
		m_x = x;
	}

	long getY() const {
		return m_y;
	}

	void setY(long y) {
		m_y = y;
	}

protected:
	CCfits::FITS* m_fits;
	std::valarray<unsigned long> contents;

	long m_x;
	long m_y;
};

#endif /* FITSREADER_H_ */
