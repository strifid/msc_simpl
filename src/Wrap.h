/*
 * Counter.h
 *
 *  Created on: Jul 8, 2012
 *      Author: trifid
 */

#ifndef COUNTER_H_
#define COUNTER_H_
#include "Pixel.h"

class Wrap {
public:
	enum Type{
		UP_WRAP = 0, DOWN = 1, BD = 2, O = 3
	};

	Wrap();
	virtual ~Wrap();

	void addPoint(const Pixel& pt);
	void removePoint(const Pixel& pt);
	bool isInWrap(const Pixel& pt);
	bool isEmpty(){
		return m_points.empty();
	}

	PointsMap& points();
	bool operator==(Wrap& wrap);

	uint32_t singleConnectedCount();
protected:

	PointsMap m_points;

};

#endif /* COUNTER_H_ */
