/*
 * Subset.h
 *
 *  Created on: Jul 8, 2012
 *      Author: trifid
 */

#ifndef SUBSET_H_
#define SUBSET_H_
#include "opencv2/core/core.hpp"
#include <mt/logger/Logable.h>
#include "Wrap.h"

class Image;

using cv::Point;

class Subset: public mt::Logable {
public:
	Subset();
	virtual ~Subset();
	void addPoint(const Pixel& pt);
	PointsMap &points();
	bool isInSubset(const Pixel& pt);
	bool isInSubsetAndWraps(const Pixel& pt);
	//todo return reference. count counter dynamicly
	Pixels getContour();
	void parentImage(Image* img);

	Pixels& oneConnectedPixels();


	Pixel center(){
		return m_center;
	}
	void center(const Pixel& val){
		m_center = val;
		addPoint(val);
		findWraps();

		m_wraps[Wrap::O] = m_wrap0;
		m_wraps[Wrap::UP_WRAP] = m_wrapUp;
		m_wraps[Wrap::DOWN] = m_wrapDown;
		m_wraps[Wrap::BD] = m_bd;

	}


	Wrap& wrap(Wrap::Type type){
		return m_wraps[type];
	}

protected:

	void findWraps();
	Wrap& wrap0() {
		return m_wrap0;
	}
	Wrap& wrapUp() {
		return m_wrapUp;
	}
	Wrap& wrapDown() {
		return m_wrapDown;
	}

	Wrap& bd() {
		return m_bd;
	}
	Wrap m_wraps[4];


	Pixel m_center;
	Image* m_parentImage;
	PointsMap m_points;

	Wrap m_bd;
	Wrap m_wrap0;
	Wrap m_wrapUp;
	Wrap m_wrapDown;

	Pixels m_oneConnectedPixels;
};

typedef std::vector<Subset> Subsets;

#endif /* SUBSET_H_ */
