/*
 * PPoint.cpp
 *
 *  Created on: Sep 28, 2013
 *      Author: trifid
 */

#include "PPoint.h"

uint32_t PPoint::pointId = 1;

PPoint::PPoint(int x, int y, int value, Type t) :
		Vertex(x, y) {

	m_id = pointId;
	pointId++;
	m_value = value;
	m_type = t;
	m_dim = 0;

}

PPoint::~PPoint() {
}

