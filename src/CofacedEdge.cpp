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
	m_edge = face;
	m_vertex = vtx;
}

CofacedEdge::~CofacedEdge() {
}

void CofacedEdge::draw(Mat & img) {

/*
	if ((m_edge->m_a->x == Image::m_width - 1 || m_edge->m_b->x == Image::m_width - 1) && (m_edge->m_a->x == 0 || m_edge->m_b->x == 0))
		;
*/

	Point b = m_edge->centralPoint();
	Point a(m_vertex->x * Image::m_enlargeFactor + Image::m_enlargeFactor, m_vertex->y * Image::m_enlargeFactor + Image::m_enlargeFactor);
	drawArrow(img, a, b);
}

