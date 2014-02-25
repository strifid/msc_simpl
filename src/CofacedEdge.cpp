/*
 * CofacedFace.cpp
 *
 *  Created on: Dec 4, 2012
 *      Author: trifid
 */

#include "CofacedEdge.h"
#include "opencv2/core/core.hpp"
#include "Image.h"
#include <cmath>
#include "opencv2/core/core_c.h"

using cv::line;
using cv::Scalar;
using std::atan2;

CofacedEdge::CofacedEdge(VertexPtr vtx, EdgePtr face) {
	m_face = face;
	m_vertex = vtx;
}

CofacedEdge::~CofacedEdge() {
}

void CofacedEdge::draw(Mat & img) {

	Point b = m_face->centralPoint();
	Point a(m_vertex->x * Image::m_enlargeFactor + Image::m_enlargeFactor, m_vertex->y * Image::m_enlargeFactor + Image::m_enlargeFactor);
	drawArrow(img, a, b);
}

