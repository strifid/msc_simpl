/*
 * Drawer.h
 *
 *  Created on: Jun 19, 2014
 *      Author: trifid
 */

#ifndef DRAWER_H_
#define DRAWER_H_
#include "opencv2/core/core.hpp"

using cv::Mat;

class Drawer {

public:
	virtual ~Drawer() {
	}
	virtual void draw(Mat& mat) = 0;

};

#endif /* DRAWER_H_ */
