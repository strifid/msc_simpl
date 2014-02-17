/*
 * Counter.cpp
 *
 *  Created on: Jul 8, 2012
 *      Author: trifid
 */

#include "Wrap.h"

Wrap::Wrap() {
	// TODO Auto-generated constructor stub

}

Wrap::~Wrap() {
}

void Wrap::addPoint(const Pixel& pt) {
	m_points[pt] = true;
}

void Wrap::removePoint(const Pixel & pt) {
	m_points.erase(pt);
}

bool Wrap::isInWrap(const Pixel& pt) {
	PointsMap::iterator it = m_points.find(pt);
	if (it != m_points.end())
		return true;
	return false;

}

PointsMap & Wrap::points() {
	return m_points;
}

bool Wrap::operator ==(Wrap& wrap) {
	return m_points == wrap.points();
}

uint32_t Wrap::singleConnectedCount() {

//	Subset

/*
	do{



	}while();
*/

}


