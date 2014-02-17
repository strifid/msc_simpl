/*
 * Face.cpp
 *
 *  Created on: Nov 24, 2012
 *      Author: trifid
 */

#include "Edge.h"
#include "opencv2/core/core.hpp"
#include "Image.h"

using cv::Scalar;
using cv::line;

Edge::Edge() {
}

Edge::~Edge() {
}

Edge::Edge(VertexPtr a, VertexPtr b) {

	if (*a > *b) {
		m_a = a;
		m_b = b;
	} else {
		m_a = b;
		m_b = a;
	}

	if (m_a->value() > m_b->value()) {
		m_valueFirst = m_a->value();
		m_valueSecond = m_a->value() + m_b->value();
	} else {
		m_valueFirst = m_b->value();
		m_valueSecond = m_a->value() + m_b->value();
	}
}

bool Edge::operator <(const Edge& face) const {

	if (*m_a == *face.m_a) {
		return *m_b < *face.m_b;
	}
	return *m_a < *face.m_a;
}

bool Edge::operator ==(const Edge & face) const {
	if (*m_a == *(face.m_a))
		return *m_b == *(face.m_b);
	return false;
}

bool Edge::operator !=(const Edge & face) const {
	return !operator==(face);
}

Point Edge::centralPoint() {
	uint32_t x1 = m_a->x * Image::m_enlargeFactor + Image::m_enlargeFactor;
	uint32_t y1 = m_a->y * Image::m_enlargeFactor + Image::m_enlargeFactor;

	uint32_t x2 = m_b->x * Image::m_enlargeFactor + Image::m_enlargeFactor;
	uint32_t y2 = m_b->y * Image::m_enlargeFactor + Image::m_enlargeFactor;

	return Point((x1 + x2) / 2, (y1 + y2) / 2);
}

void Edge::draw(Mat& img, int32_t thickness, Scalar color) const {

	int lineType = 8;
	Point a((m_a->x + 1) * Image::m_enlargeFactor, (m_a->y + 1) * Image::m_enlargeFactor);
	Point b((m_b->x + 1) * Image::m_enlargeFactor, (m_b->y + 1) * Image::m_enlargeFactor);

	line(img, a, b, color, thickness, lineType);

}

std::ostream & operator <<(std::ostream & out, Edge & vt) {
	out << "face: " << *vt.m_a << "-" << *vt.m_b;
	return out;
}

VertexPtr Edge::getDifferentVertex(Edge *face) {
	if (*(face->m_a) == *m_a)
		return m_b;
	return m_a;
}
VertexPtr Edge::findCommonVertex(Edge *face) {
	if (*(face->m_a) == *m_a || *(face->m_b) == *m_a)
		return m_a;
	if (*(face->m_b) == *m_b || *(face->m_a) == *m_b)
		return m_b;
	return NULL;
}

