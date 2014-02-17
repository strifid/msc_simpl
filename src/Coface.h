/*
 * Coface.h
 *
 *  Created on: Dec 4, 2012
 *      Author: trifid
 */

#ifndef COFACE_H_
#define COFACE_H_
#include "opencv2/core/core.hpp"

using cv::Mat;
using cv::Point;

class Coface {
public:
	Coface();
	virtual ~Coface();
	virtual void draw(Mat& img) = 0;
protected:

	void drawArrow(Mat& img, Point& a, Point &b);
};

typedef Coface* CofacePtr;
typedef std::vector<CofacePtr> Cofaces;

#endif /* COFACE_H_ */
