/*
 * PPoint.cpp
 *
 *  Created on: Sep 28, 2013
 *      Author: trifid
 */

#include "PPoint.h"

uint32_t PPoint::pointId = 1;

PPoint::PPoint(Vertex &vtx, Type t, uint32_t dim) :
		Vertex(0, 0) {

	m_id = pointId;
	pointId++;
	m_value = vtx.value();
	m_type = t;
	m_dim = dim;
	x = vtx.x;
	y = vtx.y;
	m_smplx = NULL;
}

PPoint::PPoint(int vtx, Type t, uint32_t dim) :
		Vertex(0, 0) {

	m_id = pointId;
	pointId++;
	m_value = vtx;
	m_type = t;
	m_dim = dim;
	x = 0;
	y = 0;
	m_smplx = NULL;
}
PPoint::~PPoint() {
}

uint32_t PPoint::getId(VertexPtr vtx) {
	std::map<VertexPtr, uint32_t, VertexPtrComparator>::iterator it = m_vrtx2Id.find(vtx);
	if (it == m_vrtx2Id.end())
		return 0;
	return it->second;
}
