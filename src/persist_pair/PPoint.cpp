/*
 * PPoint.cpp
 *
 *  Created on: Sep 28, 2013
 *      Author: trifid
 */

#include "PPoint.h"

uint32_t PPoint::pointId = 1;

PPoint::PPoint(int value, Type t, uint32_t dim) :
		Vertex(0, 0) {

	m_id = pointId;
	pointId++;
	m_value = value;
	m_type = t;
	m_dim = dim;

}

PPoint::~PPoint() {
}

